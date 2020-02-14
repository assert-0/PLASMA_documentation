#ifdef DLL_BUILD
	#define DLL __declspec(dllexport)
#else
	#define DLL __declspec(dllimport)
#endif //DLL_BUILD

extern "C"
{
	DLL void Calculate_hardware_accelerated(int num_ops, float *theta, float *point_distance, float j, float inner_radius, float length, float thickness, float inc_inner_radius, float inc_length, float inc_fi, float *bh = nullptr, float *bz = nullptr, float *b_abs = nullptr);
}