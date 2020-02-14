#include <cstdio>
#include <windows.h>
#include <ctime>

#define DLL_BUILD

#include "hardware_acceleration.h"

#define PI 3.14159265357989323
#define MI 0.0000001
#define NTHREADS 1024
#define REDUCTIONFACTOR (NTHREADS * 2)
#define NREPEATS (150 * 150)
#define CHUNKSIZE (150 * 10)

#define TYPE float

#define FREQUENCY 1

#define INPUT 0
#define STATUS 0
#define DEBUG_COPPER_EXTENSION 0

long long freq;

const int nFi = 12;
const int nA = 20;
const int nB = 60;

struct param
{
	param(){ZeroMemory(this, sizeof(*this));}
	int nA;
	int nB;
	int nFi;
	TYPE j;
	TYPE Theta;
	TYPE IncFi;
	TYPE IncA;
	TYPE IncB;
	TYPE a;
	TYPE b;
	TYPE R;
	TYPE r;
	TYPE f;
	TYPE DWeights[4][4];
	TYPE FiWeights[::nFi + 1];
	TYPE EdgeList1[::nA + 1];
	TYPE EdgeList2[::nB + 1];
	TYPE *res;
};

__global__
void Calculate_e(param par)
{
	int index = threadIdx.x;
	int global_index = blockIdx.x * blockDim.x + index;
	
    TYPE G = 0, E = 0, F = 0, eR = 0, er = 0, C = 0;
	int ii = 0, ji = 0, ki = 0;
	TYPE Temp = 0;

    TYPE z = par.r * cos(par.Theta);
    TYPE q = par.r * sin(par.Theta);
    TYPE C0 = par.IncA * par.IncB * par.j * MI * par.IncFi;
    TYPE bh = par.b / 2;
	
	TYPE dR = 0;
	TYPE dr = -bh;
	TYPE dFi = 0;
	dR += TYPE(((global_index / (par.nFi + 1) / (par.nB + 1)) % (par.nA + 1)) * par.IncA);
	if(dR == 0 && global_index > (par.nFi + 1) * (par.nB + 1))
		return;
	dr += TYPE(((global_index / (par.nFi + 1)) % (par.nB + 1)) * par.IncB);
	dFi += TYPE((global_index % (par.nFi + 1)) * par.IncFi);
	
	eR = par.R + dR;
	ii = int(round(dR / par.IncA));
	C = C0 * eR;
	F = 2 * eR * q;
	er = z + dr;
	ji = int(round((dr + bh) / par.IncB));
	E = q * q + eR * eR + er * er;
	
	if(ii != par.nA && ii != 0 && ji != par.nB && ji != 0)
		Temp = par.DWeights[ii % 4][ji % 4];
	else if(ii == par.nA || ii == 0)
		Temp = par.EdgeList2[ji];
	else
		Temp = par.EdgeList1[ii];
	
	ki = int(round(dFi / par.IncFi));
	G = cos(dFi);
	par.res[global_index] = Temp * par.FiWeights[ki] * C * (G / (sqrt(E - F * G)));
}

__device__ 
void Warp_reduce(volatile TYPE *ar, unsigned int idx) {
	if (NTHREADS >= 64) ar[idx] += ar[idx + 32];
	if (NTHREADS >= 32) ar[idx] += ar[idx + 16];
	if (NTHREADS >= 16) ar[idx] += ar[idx + 8];
	if (NTHREADS >= 8) ar[idx] += ar[idx + 4];
	if (NTHREADS >= 4) ar[idx] += ar[idx + 2];
	if (NTHREADS >= 2) ar[idx] += ar[idx + 1];
}

__global__ 
void Cut_buffer(unsigned int n, TYPE *dest_buff, TYPE *src_buff) {
	__shared__ TYPE ar[NTHREADS];
	unsigned int idx = threadIdx.x;
	unsigned int i = blockIdx.x * (NTHREADS * 2) + idx;
	unsigned int grid_size = NTHREADS * 2 * gridDim.x;
	ar[idx] = 0;
	if(i >= n) return;
	while (i < n) 
	{ 
		ar[idx] += src_buff[i];
		if(i + NTHREADS < n)
			ar[idx] += src_buff[i + NTHREADS]; 
		i += grid_size; 
	}
	__syncthreads();
	if (NTHREADS >= 1024) { if (idx < 512) { ar[idx] += ar[idx + 512]; } __syncthreads(); }
	if (NTHREADS >= 512) { if (idx < 256) { ar[idx] += ar[idx + 256]; } __syncthreads(); }
	if (NTHREADS >= 256) { if (idx < 128) { ar[idx] += ar[idx + 128]; } __syncthreads(); }
	if (NTHREADS >= 128) { if (idx < 64) { ar[idx] += ar[idx + 64]; } __syncthreads(); }
	if (idx < 32) Warp_reduce(ar, idx);
	if (idx == 0) dest_buff[blockIdx.x] = ar[0];
}

void Cut_multiple_buffers(unsigned int src_size, TYPE *dest_buff, TYPE *src_buff, unsigned int buffer_size)
{
	int cycles = src_size / buffer_size;
	for(int a = 0; a < cycles; a++)
	{
		Cut_buffer<<<buffer_size / NTHREADS / 2 + 1, NTHREADS>>>(buffer_size,
																 dest_buff + a * int(buffer_size / NTHREADS / 2 + 1),
																 src_buff + a * buffer_size);
	}
	cudaDeviceSynchronize();
}

void Calculate_single_point(param par, int blocks, TYPE *rese)
{
	if(par.r < 0.0)
	{
		cudaMemset(rese, 0, blocks * NTHREADS * sizeof(TYPE));
		return;
	}
	
	par.res = rese;
	Calculate_e<<<blocks, NTHREADS>>>(par);
	
}
	
int last_alloc_size = 0;
TYPE *ine = nullptr, *result_transfer_buff = nullptr, *local_buff = nullptr;
TYPE DWeights[4][4];
TYPE FiWeights[nFi + 1];
TYPE EdgeList1[nA + 1];
TYPE EdgeList2[nB + 1];
TYPE FirstMultiplier = 506.25, SecondMultiplier = 11.25;
TYPE Moderator = FirstMultiplier * SecondMultiplier;

void Resource_cleanup()
{
	free(local_buff);
	cudaFree(ine);
	cudaFree(result_transfer_buff);
	local_buff = nullptr;
	ine = nullptr;
	result_transfer_buff = nullptr;
}

void Resource_startup(param *par, int num_ops)
{
	Resource_cleanup();
	int dimx, dimy, dimz, blocks;
	dimx = par->nA + 1;
	dimy = par->nB + 1;
	dimz = par->nFi + 1;
	blocks = (dimx * dimy * dimz) / NTHREADS + 1;
	cudaMalloc(&ine, num_ops * sizeof(TYPE) * blocks * NTHREADS);
	cudaMalloc(&result_transfer_buff, num_ops * sizeof(TYPE) * int(blocks * NTHREADS / REDUCTIONFACTOR + 1));
	local_buff = (TYPE*)malloc(num_ops * sizeof(TYPE) * int(blocks * NTHREADS / REDUCTIONFACTOR + 1));
	last_alloc_size = num_ops * sizeof(TYPE) * blocks * NTHREADS;
}

void Prepare_weights(param par)
{
	for(int k = 0; k <= nFi; k++)
	{
		if(k == nFi || k == 0)
			FiWeights[k] = 7;
		else if(k % 4 == 0)
			FiWeights[k] = 14;
		else if(k % 4 == 1 || k % 4 == 3)
			FiWeights[k] = 32;
		else if(k % 4 == 2)
			FiWeights[k] = 12;
	}
	for(int ii = 0; ii < 4; ii++)
	{
		for(int ij = 0; ij < 4; ij++)
		{
			if(ij % 4 == 0 && ii % 4 == 0)																		
				DWeights[ii][ij] = 196 / Moderator;
			else if(ii % 4 == 0 && ij % 4 == 1 || ii % 4 == 1 && ij % 4 == 0 || ii % 4 == 3 && ij % 4 == 0 || ii % 4 == 0 && ij % 4 == 3)	
				DWeights[ii][ij] = 448 / Moderator;
			else if(ii % 4 == 2 && ij % 4 == 0 || ii % 4 == 0 && ij % 4 == 2)
				DWeights[ii][ij] = 168 / Moderator;
			else if(ii % 4 == 1 && ij % 4 == 1 || ii % 4 == 3 && ij % 4 == 3 || ii % 4 == 3 && ij % 4 == 1 || ii % 4 == 1 && ij % 4 == 3)
				DWeights[ii][ij] = 1024 / Moderator;
			else if(ii % 4 == 2 && ij % 4 == 1 || ii % 4 == 1 && ij % 4 == 2 || ii % 4 == 3 && ij % 4 == 2 || ii % 4 == 2 && ij % 4 == 3)
				DWeights[ii][ij] = 384 / Moderator;
			else if(ij % 4 == 2 && ii % 4 == 2)
				DWeights[ii][ij] = 144 / Moderator;
		}
	}
	for(int m = 0; m <= nA; m++)
	{
		if(m == 0 || m == nA)
			EdgeList1[m] = 49 / Moderator;
		else if(m % 2 == 1)
			EdgeList1[m] = 224 / Moderator;
		else if(m % 4 == 0)		
			EdgeList1[m] = 98 / Moderator;
		else if(m % 4 == 2)
			EdgeList1[m] = 84 / Moderator;
	}
	for(int n = 0; n <= par.nB; n++)
	{
		if(n == 0 || n == par.nB)		
			EdgeList2[n] = 49 / Moderator;
		else if(n % 2 == 1)
			EdgeList2[n] = 224 / Moderator;
		else if(n % 4 == 0)
			EdgeList2[n] = 98 / Moderator;
		else if(n % 4 == 2)		
			EdgeList2[n] = 84 / Moderator;
	}
}

void Copy_ar(TYPE *ar1, TYPE *ar2, int size)
{
	memcpy(ar1, ar2, size * sizeof(TYPE));
}

void Calculate_hardware_accelerated(int num_ops, TYPE *theta, TYPE *point_distance, 
									TYPE j, TYPE inner_radius, TYPE length, 
									TYPE thickness, TYPE inc_thickness, TYPE inc_length, 
									TYPE inc_fi, TYPE *E, TYPE *deprecated_1, TYPE *deprecated_2)
{
	long long start, stop;
	
	int dimx, dimy, dimz, blocks;
	param par;
	par.nA = nA;
	par.nB = nB;
	par.nFi = nFi;
	par.j = j;
	par.IncFi = PI / nFi;
	par.IncA = thickness / nA;
	par.IncB = length / nB;
	par.a = thickness;
	par.b = length;
	par.R = inner_radius;
	par.f = FREQUENCY;
	
	dimx = par.nA + 1;
	dimy = par.nB + 1;
	dimz = par.nFi + 1;
	blocks = (dimx * dimy * dimz) / NTHREADS + 1;
	
	Prepare_weights(par);
	for(int it1 = 0; it1 < 4; it1++)
	{
		for(int it2 = 0; it2 < 4; it2++)
			par.DWeights[it1][it2] = DWeights[it1][it2];
	}
	Copy_ar(par.FiWeights, FiWeights, nFi + 1);
	Copy_ar(par.EdgeList1, EdgeList1, nA + 1);
	Copy_ar(par.EdgeList2, EdgeList2, nB + 1);
	
	if(last_alloc_size < num_ops * sizeof(TYPE) * blocks * NTHREADS)
	{
		Resource_startup(&par, num_ops);
	}
	
	#if DEBUG_COPPER_EXTENSION
		QueryPerformanceCounter((LARGE_INTEGER*)&start);
	#endif
	for(int a = 0; a < num_ops; a++)
	{
		par.Theta = theta[a];
		par.r = point_distance[a];
		Calculate_single_point(par, blocks, ine + a * blocks * NTHREADS);
	}
	cudaDeviceSynchronize();
	#if DEBUG_COPPER_EXTENSION 
		QueryPerformanceCounter((LARGE_INTEGER*)&stop); 
		printf("	Calculations: %.15g s\n", double(stop - start) / double(freq));
	#endif
	
	
	
	#if DEBUG_COPPER_EXTENSION
		printf("	E\n");
	#endif
	if(E != nullptr)
	{
		#if DEBUG_COPPER_EXTENSION 
			QueryPerformanceCounter((LARGE_INTEGER*)&start);
		#endif
		Cut_multiple_buffers(num_ops * blocks * NTHREADS, result_transfer_buff, ine, blocks * NTHREADS);
		#if DEBUG_COPPER_EXTENSION 
			QueryPerformanceCounter((LARGE_INTEGER*)&stop);
			
			printf("		Buffer reduction: %.15g s\n", double(stop - start) / double(freq));
		
			QueryPerformanceCounter((LARGE_INTEGER*)&start);
		#endif
		cudaMemcpy(local_buff, result_transfer_buff, num_ops * sizeof(TYPE) * int(blocks * NTHREADS / REDUCTIONFACTOR + 1), cudaMemcpyDeviceToHost);
		for(int a = 0; a < num_ops; a++)
		{
			E[a] = 0;
			for(int b = 0; b < int(blocks * NTHREADS / REDUCTIONFACTOR + 1); b++)
			{
				E[a] += local_buff[a * int(blocks * NTHREADS / REDUCTIONFACTOR + 1) + b];
			}
			E[a] *= 2 * PI * par.f;
			deprecated_1[a] = 0;
			deprecated_2[a] = 0;
		}
		#if DEBUG_COPPER_EXTENSION 
			QueryPerformanceCounter((LARGE_INTEGER*)&stop);
			
			printf("		Data loading and processing: %.15g s\n", double(stop - start) / double(freq));
		#endif
	}



	#if DEBUG_COPPER_EXTENSION
		printf("	Buff1 size: %d\n", num_ops * int(sizeof(TYPE)) * blocks * NTHREADS);
		printf("	Buff2 size: %d\n", num_ops * int(sizeof(TYPE)) * int(blocks * NTHREADS / REDUCTIONFACTOR + 1));
		printf("	Reduction factor: %d\n", REDUCTIONFACTOR);
		printf("	Payload size: %d\n", num_ops * int(sizeof(TYPE)) * int(blocks * NTHREADS / REDUCTIONFACTOR + 1));
		printf("	Threads per calculation: %d\n", blocks * NTHREADS);
	#endif
}

TYPE j = 1000000;
TYPE inner_radius = 0.03;
TYPE thickness = 0.03;
TYPE length = 0.12;
TYPE alpha = PI / 6;

TYPE point_distance = 0.025;
TYPE theta = PI / 2;

TYPE inc_fi = PI / nFi;
TYPE inc_thickness = thickness / nA;
TYPE inc_length = length / nB;

TYPE *E, *filler;

TYPE *distance_ar, *theta_ar;

#if INPUT == 1

	void Input()
	{
		FILE *f = fopen("input.dat", "point_distance");
		fscanf(f, " j=%f", &j);
		fscanf(f, " point_distance=%f", &point_distance);
		fscanf(f, " inner_radius=%f", &inner_radius);
		fscanf(f, " thickness=%f", &thickness);
		fscanf(f, " length=%f", &length);
		fscanf(f, " theta=%f", &theta);
		fscanf(f, " alpha=%f", &alpha);
		fscanf(f, " inc_inner_radius=%f", &inc_thickness);
		fscanf(f, " inc_length=%f", &inc_length);
		fscanf(f, " inc_fi=%f", &inc_fi);
	}
	
#else

	void Input()
	{
		
	}

#endif

void Setup()
{
	E = new TYPE [NREPEATS];
	filler = new TYPE [NREPEATS];
	ZeroMemory(E, NREPEATS * sizeof(TYPE));
	distance_ar = new TYPE [NREPEATS];
	theta_ar = new TYPE [NREPEATS];
	for(int a = 0; a < NREPEATS; a++)
	{
		distance_ar[a] = point_distance;
		theta_ar[a] = theta;
	}
}

int main()
{
	Input();
	Setup();
	long long start, stop, start2, stop2;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&start);
	for(int a = 0; a < NREPEATS / CHUNKSIZE; a++)
	{
		if(STATUS)
		{
			system("cls");
			printf("%.2f%% done...\n", (float)a / (NREPEATS / CHUNKSIZE) * 100.0);
		}
		#if DEBUG_COPPER_EXTENSION
			printf("Pass: %d\n", a);
			QueryPerformanceCounter((LARGE_INTEGER*)&start2);
		#endif
		Calculate_hardware_accelerated(CHUNKSIZE, theta_ar + a * CHUNKSIZE, 
									   distance_ar + a * CHUNKSIZE, j, inner_radius, 
									   length, thickness, inc_thickness, inc_length, 
									   inc_fi, E + a * CHUNKSIZE, filler + a * CHUNKSIZE, filler + a * CHUNKSIZE);
		#if DEBUG_COPPER_EXTENSION
			QueryPerformanceCounter((LARGE_INTEGER*)&stop2);
			printf("Elapsed time: %.15g s\n", double(stop2 - start2) / double(freq));
		#endif
	}
	QueryPerformanceCounter((LARGE_INTEGER*)&stop);
	
	Resource_cleanup();
	
	printf("Done!\n");
	printf("%.15f\n", E[NREPEATS - 1]);
	printf("Average time expended per operation: %.15g s\n", double(stop - start) / double(freq) / NREPEATS);
	printf("Total time expended: %.15g s\n", double(stop - start) / double(freq));
	system("pause");
	
	
	
//	--------------------------
	
	
	
	/*
	const int factor = NTHREADS * 2, buff1_size = 19200, buff2_size = buff1_size / factor == 0 ? 1 : buff1_size / factor + 1;
	printf("buff2 %d\n", buff2_size);
	TYPE res = 0;
	TYPE *loc1, *loc2, *dev1, *dev2;
	loc1 = (TYPE*)malloc(buff1_size * sizeof(TYPE));
	loc2 = (TYPE*)malloc(buff2_size * sizeof(TYPE));
	cudaMalloc(&dev1, buff1_size * sizeof(TYPE));
	cudaMalloc(&dev2, buff2_size * sizeof(TYPE));
	for(int a = 0; a < buff1_size; a++)
		loc1[a] = 1;
	ZeroMemory(loc2, buff2_size * sizeof(TYPE));
	cudaMemcpy(dev1, loc1, buff1_size * sizeof(TYPE), cudaMemcpyHostToDevice);
	cudaMemset(dev2, 0, buff2_size * sizeof(TYPE));
	for(int a = 0; a < 25000; a++)
		Cut_buffer<<<buff2_size + 2, NTHREADS>>>(buff1_size, dev2, dev1);
	cudaDeviceSynchronize();
	cudaMemcpy(loc2, dev2, buff2_size * sizeof(TYPE), cudaMemcpyDeviceToHost);
	for(int a = 0; a < buff2_size; a++)
		res += loc2[a];
	printf("%.15g\n", res);
	*/
	
	
	
	/*
	const unsigned int factor = NTHREADS * 2, chunk_size = 57600, buff1_size = chunk_size * CHUNKSIZE;
	const unsigned int buff2_size = (chunk_size / factor + 1) * CHUNKSIZE;
	long long start, stop;
	printf("%d\n", buff2_size);
	TYPE res = 0;
	TYPE *loc1, *loc2, *dev1, *dev2;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	loc1 = (TYPE*)malloc(buff1_size * sizeof(TYPE));
	loc2 = (TYPE*)malloc(buff2_size * sizeof(TYPE));
	cudaMalloc(&dev1, buff1_size * sizeof(TYPE));
	cudaMalloc(&dev2, buff2_size * sizeof(TYPE));
	for(int a = 0; a < buff1_size; a++)
		loc1[a] = 1;
	ZeroMemory(loc2, buff2_size * sizeof(TYPE));
	cudaMemcpy(dev1, loc1, buff1_size * sizeof(TYPE), cudaMemcpyHostToDevice);
	cudaMemset(dev2, 0, buff2_size * sizeof(TYPE));
	QueryPerformanceCounter((LARGE_INTEGER*)&start);
	Cut_multiple_buffers(buff1_size, dev2, dev1, chunk_size);
	cudaMemcpy(loc2, dev2, buff2_size * sizeof(TYPE), cudaMemcpyDeviceToHost);
	for(unsigned int a = 0; a < buff2_size; a++)
	{
		res += loc2[a];
	}
	res /= CHUNKSIZE;
	QueryPerformanceCounter((LARGE_INTEGER*)&stop);
	
	printf("res: %.15g\n", res);
	printf("%.15g s\n", double(stop - start) / double(freq));
	*/
	
	
	
	/*
	float tmp_ar[2048];
	ZeroMemory(tmp_ar, 2048 * sizeof(int));
	for(int a = 0; a < buff2_size; a++)
	{
		for(int b = 0; b < 2048; b += 2)
		{
			if(tmp_ar[b] == loc2[a])
			{
				tmp_ar[b + 1]++;
				break;
			}
			else if(tmp_ar[b] == 0)
			{
				tmp_ar[b] = loc2[a];
				tmp_ar[b + 1] = 1;
				break;
			}
		}
	}
	*/
	
	
	
	/*
	for(int b = 0; b < 2048; b += 2)
	{
		if(tmp_ar[b] == 0)
			break;
		printf("key: %f value: %f\n", tmp_ar[b], tmp_ar[b + 1]);
	}
	*/
	
	
	
	/*
	TYPE *loc1, *dev1, *dev2;
	TYPE res;
	int buff_size = 1027;
	
	loc1 = (TYPE*)malloc(buff_size * sizeof(TYPE));
	cudaMalloc(&dev1, buff_size * sizeof(TYPE));
	cudaMalloc(&dev2, buff_size * sizeof(TYPE));
	
	for(int a = 0; a < buff_size; a++)
		loc1[a] = 1;
	
	cudaMemcpy(dev1, loc1, buff_size * sizeof(TYPE), cudaMemcpyHostToDevice);
	cudaMemcpy(dev2, loc1, buff_size * sizeof(TYPE), cudaMemcpyHostToDevice);
	cudaMemset(dev2, 0, buff_size * sizeof(TYPE));
	
	Reduce_buffer(buff_size, &res, dev1, dev2);
	
	printf("%.15g\n", res);
	*/
	
	
	
	/*
	cudaFree(dev1);
	cudaFree(dev2);
	free(loc1);
	free(loc2);
	*/
}