#include "ctpl.h"

//coil class and threadpool
const Type Moderator=5695.3125;

class Coil;

class thread_pool : public ctpl::thread_pool
{
public:
    thread_pool(){Init();}

    thread_pool(int n){Init(n);}

    void Init(int n=0)
    {
        if(!n)
        {
            SYSTEM_INFO info;
            GetSystemInfo(&info);
            resize(info.dwNumberOfProcessors);
        }
        else
            resize(n);
    }

    void Set_priority(int priority)
    {
        for(int it=0; it < this->size(); it++)
        for(int it=0; it < this->size(); it++)
            SetThreadPriority(reinterpret_cast<HANDLE>(this->get_thread(it).native_handle()), priority);
    }
} tp;

struct thread_param
{
    Coil *c;
    Type r;
    Type Theta;
    Type dR_start;
    Type dR_stop;
    Type res;
    bool completed;
};

//magnetic coil specs
class Coil {

	public:

		Type j;		//Current density, calculated from current and d
		Type R;		//Internal coio radius
		Type a;		//Coil thickness
		Type b;		//Coil length
		Type d;		//Coil winding wire thickness
		Type dis;	//Displacement of the coil along the x axis
		int nA;		//Number of precision increments over a
		Type IncA;	//Precision increment over a
		int nB;		//Number of precision increments over b
		Type IncB;	//Precision increment over b
		int nFi;	//Number of precision increments over angle Phi
		Type IncFi;	//Precision increment over angle, usually fixed to 12
		bool isEM;	//Determines if the coil should generate a magnetic field
		Type f;		//Frequency of sinusoidal oscillation in EM coils
		Type Ro;	//Resistivity of a coil, material dependent (1.63e-8 for copper)
		Type L;		//Self inductance of a coil, calculated later
		Type Res;	//Resistance, real part of impedence, skin effect compensated for AC fields
		Type Reac;	//Reactance, imaginary part of impedence, dependent on f and L
		Type Imp;	//Impedence, total dissipative potential in AC fields

		Type DWeights[4][4];
		vector<Type> FiWeights;
		vector<Type> EdgeList1;
		vector<Type> EdgeList2;

		//Constructor with all coil characteristics accounted for
		Coil (Type _I, Type _R, Type _a, Type _b, Type _d, Type _dis, int _nA, int _nB, int _nFi, bool _isEM, Type _f, Type _Ro){

			j=_I/(_d*_d);
			R=_R;
			a=_a;
			b=_b;
			d=_d;
			dis=_dis;
			nA=_nA;
			IncA=a/_nA;
			nB=_nB;
			IncB=b/_nB;
			nFi=_nFi;
			IncFi=Pi/_nFi;
			isEM=_isEM;
			Ro=_Ro;

			if (_isEM=true){
				f=_f;
			}
			else{
				f=0;
			}

			for (int k=0; k<=nFi; k++){
				if (k==nFi || k==0){		FiWeights.push_back(7);}
				else if (k%4==0){			FiWeights.push_back(14);}
				else if (k%4==1 || k%4==3){	FiWeights.push_back(32);}
				else if (k%4==2){			FiWeights.push_back(12);}
			}
			for (int ii=0; ii<4; ii++){
				for (int ij=0; ij<4; ij++){
					if (ij%4==0 && ii%4==0){																		DWeights[ii][ij]=196/Moderator;}
					else if (ii%4==0 && ij%4==1 || ii%4==1 && ij%4==0 || ii%4==3 && ij%4==0 || ii%4==0 && ij%4==3){	DWeights[ii][ij]=448/Moderator;}
					else if (ii%4==2 && ij%4==0 || ii%4==0 && ij%4==2){												DWeights[ii][ij]=168/Moderator;}
					else if (ii%4==1 && ij%4==1 || ii%4==3 && ij%4==3 || ii%4==3 && ij%4==1 || ii%4==1 && ij%4==3){	DWeights[ii][ij]=1024/Moderator;}
					else if (ii%4==2 && ij%4==1 || ii%4==1 && ij%4==2 || ii%4==3 && ij%4==2 || ii%4==2 && ij%4==3){	DWeights[ii][ij]=384/Moderator;}
					else if (ij%4==2 && ii%4==2){																	DWeights[ii][ij]=144/Moderator;}
				}
			}
			for (int m=0; m<=nA; m++){
				if (m==0 || m==nA){		EdgeList1.push_back(49/Moderator);}
				else if (m%2==1){		EdgeList1.push_back(224/Moderator);}
				else if (m%4==0){		EdgeList1.push_back(98/Moderator);}
				else if (m%4==2){		EdgeList1.push_back(84/Moderator);}
			}
			for (int n=0; n<=nB; n++){
				if (n==0 || n==nB){		EdgeList2.push_back(49/Moderator);}
				else if (n%2==1){		EdgeList2.push_back(224/Moderator);}
				else if (n%4==0){		EdgeList2.push_back(98/Moderator);}
				else if (n%4==2){		EdgeList2.push_back(84/Moderator);}
			}

			InductanceCalc();
			ResistanceCalc();
			ReactanceCalc();
			ImpedenceCalc();
		}

		//Default constructor.

		Coil(){}

		static long unsigned int BhAsyncThread(int id, void *in)
		{
		    thread_param &p = *(thread_param*)in;
		    const Coil &c = *p.c;

            Type FieldH=0;
		    Type G=0, Dh=0, E=0, F=0, eR=0, er=0;
			int ii=0, ji=0, ki=0;
			Type Temp=0;

		    Type z=p.r*cos(p.Theta);
		    Type q=p.r*sin(p.Theta);
		    Type C=c.IncA*c.IncB*c.j*Mi*c.IncFi;
		    Type bh=c.b/2;

			for (Type dR=p.dR_start; dR<p.dR_stop; dR+=c.IncA){

                //printf("id: %d, value: %g\n", id, dR);

				eR=c.R+dR;
				ii=int(round(dR/c.IncA));
				F=2*eR*q;

				for (Type dr=-bh; dr<=bh; dr+=c.IncB){

					er=z+dr;
					ji=int(round((dr+bh)/c.IncB));
			        Dh=C*eR*er;
			        E=q*q+eR*eR+er*er;

			        if (ii!=c.nA && ii!=0 && ji!=c.nB && ji!=0){	Temp=c.DWeights[ii%4][ji%4];}
					else if (ii==c.nA || ii==0){					Temp=c.EdgeList2[ji];}
					else{ 										    Temp=c.EdgeList1[ii];}

		        	for (Type dFi=0; dFi<=Pi; dFi+=c.IncFi){

				        G=cos(dFi);
				        ki=int(round(dFi/c.IncFi));
				        FieldH+=Temp*c.FiWeights[ki]*(Dh*G)/pow((E-F*G), 1.5);
			        }
			    }
			}

		    p.res = FieldH;
		    p.completed = true;
		    return 0;
		}

		static long unsigned int BzAsyncThread(int id, void *in)
		{
		    thread_param &p = *(thread_param*)in;
		    const Coil &c = *p.c;

            Type FieldZ=0;
		    Type G=0, Dz=0, Cz=0, E=0, F=0, eR=0, er=0;
			int ii=0, ji=0, ki=0;
			Type Temp=0;

		    Type z=p.r*cos(p.Theta);
		    Type q=p.r*sin(p.Theta);
		    Type C=c.IncA*c.IncB*c.j*Mi*c.IncFi;
		    Type bh=c.b/2;

		    for (Type dR=p.dR_start; dR<p.dR_stop; dR+=c.IncA){

				eR=c.R+dR;
				ii=int(round(dR/c.IncA));
				Cz=eR*eR;
		        Dz=eR*q;
		        F=2*Dz;

				for (Type dr=-bh; dr<=bh; dr+=c.IncB){

					er=z+dr;
					ji=int(round((dr+bh)/c.IncB));
					E=q*q+eR*eR+er*er;

					if (ii!=c.nA && ii!=0 && ji!=c.nB && ji!=0){	Temp=c.DWeights[ii%4][ji%4];}
					else if (ii==c.nA || ii==0){					Temp=c.EdgeList2[ji];}
					else{ 										    Temp=c.EdgeList1[ii];}

		            for (Type dFi=0; dFi<=Pi; dFi+=c.IncFi){

		            	ki=int(round(dFi/c.IncFi));
		                G=cos(dFi);
		                FieldZ+=Temp*c.FiWeights[ki]*C*((Cz-Dz*G)/(pow((E-F*G), 1.5)));
		            }
				}
			}

            p.res = FieldZ;
		    p.completed = true;
		    return 0;
		}

		static long unsigned int EAsyncThread(int id, void *in)
		{
		    thread_param &p = *(thread_param*)in;
		    const Coil &c = *p.c;

            Type FieldE=0;
			Type G=0, E=0, F=0, eR=0, er=0, C=0;
			int ii=0, ji=0, ki=0;
			Type Temp=0;

			Type z=p.r*cos(p.Theta);
			Type q=p.r*sin(p.Theta);
			Type C0=c.IncA*c.IncB*c.j*Mi*c.IncFi;
		    Type bh=c.b/2;

		    for (Type dR=p.dR_start; dR<p.dR_stop; dR+=c.IncA){

				eR=c.R+dR;
				C=C0*eR;
				ii=int(round(dR/c.IncA));
		        F=2*eR*q;

				for (Type dr=-bh; dr<=bh; dr+=c.IncB){

					er=z+dr;
					ji=int(round((dr+bh)/c.IncB));
					E=q*q+eR*eR+er*er;

					if (ii!=c.nA && ii!=0 && ji!=c.nB && ji!=0){	Temp=c.DWeights[ii%4][ji%4];}
					else if (ii==c.nA || ii==0){					Temp=c.EdgeList2[ji];}
					else{ 										Temp=c.EdgeList1[ii];}

		            for (Type dFi=0; dFi<=Pi; dFi+=c.IncFi){

		            	ki=int(round(dFi/c.IncFi));
						G=cos(dFi);
		                FieldE+=Temp*c.FiWeights[ki]*C*((G)/(sqrt(E-F*G)));
		            }
				}
			}

		    p.res = FieldE*2*Pi*c.f;
		    p.completed = true;
		    return 0;
		}

		//Magnetic field calculation methods
		Type BhCalc(Type, Type);

		Type BxCalc(Type r, Type Theta, Type Alpha){

			return BhCalc(r, Theta)*cos(Alpha);
		}

		Type ByCalc(Type r, Type Theta, Type Alpha){

			return BhCalc(r, Theta)*sin(Alpha);
		}

		Type BzCalc(Type, Type);

		//Electric field calculation methods
		Type ECalc (Type, Type);

		Type ExCalc(Type r, Type Theta, Type Alpha){

			return -ECalc(r, Theta)*sin(Alpha);
		}

		Type EyCalc(Type r, Type Theta, Type Alpha){

			return ECalc(r, Theta)*cos(Alpha);
		}

		//Inductance calculation, executed upon coil creation
		void InductanceCalc(){

			for (Type id=0; id<=b/2; id+=d){

				for (Type jd=R; jd<R+a; jd+=d){

					Type TempL=0;

					Type r=sqrt((b/2-id)*(b/2-id)+jd*jd);
					Type Theta=asin(jd/r);

					Type G=0, E=0, F=0, eR=0, er=0, C=0;
					int ii=0, ji=0, ki=0;
					Type Temp=0;

					Type z=r*cos(Theta);
					Type q=r*sin(Theta);
					Type C0=IncA*IncB*Mi*IncFi/(d*d);
				    Type bh=b/2;

				    for (Type dR=0; dR<=a; dR+=IncA){

						eR=R+dR;
						C=C0*eR;
						ii=int(round(dR/IncA));
				        F=2*eR*q;

						for (Type dr=-bh; dr<=bh; dr+=IncB){

							er=z+dr;
							ji=int(round((dr+bh)/IncB));
							E=q*q+eR*eR+er*er;

							if (int((id)/IncB)==ji and int((jd-R)/IncA)==ii){

								continue;

							}
							else{

								if (ii!=nA && ii!=0 && ji!=nB && ji!=0){	Temp=DWeights[ii%4][ji%4];}
								else if (ii==nA || ii==0){					Temp=EdgeList2[ji];}
								else{ 										Temp=EdgeList1[ii];}

					            for (Type dFi=0; dFi<=Pi; dFi+=IncFi){

					            	ki=int(round(dFi/IncFi));
									G=cos(dFi);
					                TempL+=Temp*FiWeights[ki]*C*((G)/(sqrt(E-F*G)));
					            }
							}
						}
					}
					if (isinf(TempL)==false){
						L+=TempL*2*Pi*jd;
					
					}
					else{
						printf("Shit\n");
						TempL=0;
					}
				}
			}
			L*=2;
		}

		void ResistanceCalc(){

			Type Resistance=0;

			for (Type Rad=0; Rad<a; Rad+=d){
				Resistance+=2*(R+Rad);
			}

			Type Re=Resistance*Ro*(b/d)/((d/2)*(d/2));

			Type J0=1000000;
		    Type Rel=0;
		    Type skin=sqrt(Ro/(Pi*f*Mi));
		    Type IncD=d/10000;

		    for (Type dD=0; dD<d; dD+=IncD){
		        Rel+=J0*exp(-dD/skin)*(((d-dD)*(d-dD)-(d-dD-IncD)*(d-dD-IncD)));
		    }
		    Res=Re/Rel;
		}

		void ReactanceCalc(){

			Reac=L*2*Pi*f;
		}

		void ImpedenceCalc(){

			Imp=sqrt(Res*Res+Reac*Reac);
		}

		bool operator!=(Coil &in)
		{
            if(in.j != j)
                return true;
            else if(in.R != R)
                return true;
            else if(in.a != a)
                return true;
            else if(in.b != b)
                return true;
            else if(in.d != d)
                return true;
            else if(in.dis != dis)
                return true;
            else if(in.nA != nA)
                return true;
            else if(in.IncA != IncA)
                return true;
            else if(in.nB != nB)
                return true;
            else if(in.IncB != IncB)
                return true;
            else if(in.nFi != nFi)
                return true;
            else if(in.IncFi != IncFi)
                return true;
            else if(in.isEM != isEM)
                return true;

            return false;
		}

    private:

        Type Calc(Type r, Type Theta, std::function<long unsigned int(int, void*)> func);
};

std::vector<Coil> backup_vec;
std::vector<thread_param> ar;

Type Coil::ECalc(Type r, Type Theta){

    return Calc(r, Theta, EAsyncThread);
}

Type Coil::BhCalc(Type r, Type Theta){

    return Calc(r, Theta, BhAsyncThread);
}

Type Coil::BzCalc(Type r, Type Theta){

    return Calc(r, Theta, BzAsyncThread);
}

Type Coil::Calc(double r, double Theta, std::function<long unsigned int(int, void*)> func)
{
    int stop = 0, remainder;
    Type ret = 0;
    Type tmp = 0;
    if(ar.size() != tp.size() || backup_vec[0] != *this)
    {
        backup_vec.resize(tp.size());
        for(int it = 0; it < backup_vec.size(); ++it)
        {
            backup_vec[it] = *this;
            for(int it2 = 0; it2 < 4; it2++)
                for(int it3 = 0; it3 < 4; it3++)
                    backup_vec[it].DWeights[it2][it3] = this->DWeights[it2][it3];
            backup_vec[it].FiWeights = this->FiWeights;
            backup_vec[it].EdgeList1 = this->EdgeList1;
            backup_vec[it].EdgeList2 = this->EdgeList2;
        }

        ar.resize(tp.size());
    }
    ZeroMemory(&ar.front(), ar.size() * sizeof(thread_param));
    remainder = int(Type(a) / Type(IncA)) % tp.size();
    for(int it = 0; it < tp.size(); ++it)
    {
        ar[it].r = r;
        ar[it].Theta = Theta;
        ar[it].c = &backup_vec[it];
        ar[it].completed = false;
        if(it == tp.size() - 1)
        {
            ar[it].dR_start = tmp;
            ar[it].dR_stop = a;
            break;
        }
        ar[it].dR_start = tmp;
        if(remainder)
        {
            tmp += (int(Type(a) / Type(IncA) / tp.size()) + 1) * IncA;
            --remainder;
        }
        else
            tmp += int(Type(a) / Type(IncA) / tp.size()) * IncA;
        ar[it].dR_stop = tmp;
        ar[it].dR_stop -= IncA / 4;
    }


    for(int it = 0; it < tp.size(); ++it)
        tp.push(func, &(ar[it]));

    while(stop < tp.size())
    {
        stop = 0;
        for(auto it : ar)
            if(it.completed)
                ++stop;
    }

    for(auto it : ar)
    {
        ret += it.res;
    }

    return ret;
}
