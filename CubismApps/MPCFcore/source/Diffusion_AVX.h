/*
 *  Diffusion_AVX.h
 *  MPCFcore
 *
 *  Created by Diego Rossinelli on 3/2/12.
 *  Copyright 2012 ETH Zurich. All rights reserved.
 *
 */
#pragma once

#include <xmmintrin.h>

#include "Diffusion_SSE.h"
#include "DivTensor_AVX.h"

class Diffusion_AVX: public virtual Diffusion_SSE, public virtual DivTensor_AVX
{
public:
	
	Diffusion_AVX(const Real a=1, const Real nu1 = 1, const Real nu2 = 2, const Real G1 = 1/(2.1-1), const Real G2 = 1/(2.1-1), const Real h = 1, const Real dtinvh = 1):
	DivTensor_AVX(a, dtinvh, h, 0.5), //this is the base class of Diffusion_CPP and DivTensor_SSE
	Diffusion_SSE(a, nu1, nu2, G1, G2, h, dtinvh), 
	Diffusion_CPP(a, nu1, nu2, G1, G2, h, dtinvh),
	DivTensor_CPP(a, dtinvh, h, 0.5) 
	{
	}
	
protected:

	/*
	This is commented out because on sandy bridge was going slower than SSE (ILP-issues + register spills)
	void _xface(const InputSOAf_ST& _mu,
				const TempSOAf_ST& ux0, const TempSOAf_ST& uy0, const TempSOAf_ST& uz0, const TempSOAf_ST& ux1, const TempSOAf_ST& uy1, const TempSOA_ST& uz1, 
				const TempSOAf_ST& vx0, const TempSOAf_ST& vy0, const TempSOAf_ST& vx1, const TempSOAf_ST& vy1,
				const TempSOAf_ST& wx0, const TempSOAf_ST& wz0, const TempSOAf_ST& wx1, const TempSOAf_ST& wz1)
	{	
#ifndef _SP_COMP_
		printf("Diffusion_AVX::_xface: you should not be here in double precision. Aborting.\n");
		abort();
#else
		const float factor = (float)(2./3);
		
		_average_xface<false>(2-factor, ux0, ux1, txx);
		_average_xface<true>(-factor, vy0, vy1, txx);
		_average_xface<true>(-factor, wz0, wz1, txx);
		
		_average_xface<false>(1, vx0, vx1, txy);
		_average_xface<true>(1, uy0, uy1, txy);
		
		_average_xface<false>(1, uz0, uz1, txz);
		_average_xface<true>(1, wx0, wx1, txz);
		
		static const int PITCHIN = InputSOA_ST::PITCH;
		static const int PITCHOUT = TempPiXSOA_ST::PITCH;
		
		const float * const mubase = _mu.ptr(0,0); 
		float * const txxbase = &txx.ref(0,0); 
		float * const txybase = &txy.ref(0,0); 
		float * const txzbase = &txz.ref(0,0); 

		for(int iy=0; iy<TempPiXSOA_ST::NY; iy++)
		{
			const float * const muptr = mubase + iy*PITCHIN; 
			float * const txxptr = txxbase + iy*PITCHOUT;
			float * const txyptr = txybase + iy*PITCHOUT;
			float * const txzptr = txzbase + iy*PITCHOUT;
			
			for(int ix=0; ix<TempPiXSOA_ST::NX; ix+=8)
			{
				const __m128 W =  _mm_load_ps(muptr + ix - 4);
				const __m256 C = _mm256_load_ps(muptr + ix);
				const __m256 mu = C + _LEFT(W,C);
				
				_mm256_store_ps(txxptr + ix, mu*_mm256_load_ps(txxptr + ix));
				_mm256_store_ps(txyptr + ix, mu*_mm256_load_ps(txyptr + ix));
				_mm256_store_ps(txzptr + ix, mu*_mm256_load_ps(txzptr + ix));
			}
		}
#endif
	}*/
	
	void _yface(const InputSOAf_ST& _mu,
				const TempSOAf_ST& ux0, const TempSOAf_ST& uy0, const TempSOAf_ST& ux1, const TempSOAf_ST& uy1, 
				const TempSOAf_ST& vx0, const TempSOAf_ST& vy0, const TempSOAf_ST& vz0, const TempSOAf_ST& vx1, const TempSOAf_ST& vy1, const TempSOA_ST& vz1,
				const TempSOAf_ST& wy0, const TempSOAf_ST& wz0, const TempSOAf_ST& wy1, const TempSOAf_ST& wz1)
	{		 
#ifndef _SP_COMP_
		printf("Diffusion_AVX::_yface: you should not be here in double precision. Aborting.\n");
		abort();
#else
		const float factor = (float)(2./3);
		
		_average_yface<false>(1, uy0, uy1, tyx);
		_average_yface<true>(1, vx0, vx1, tyx);

		_average_yface<false>(-factor, ux0, ux1, tyy);
		_average_yface<true>(2-factor, vy0, vy1, tyy);
		_average_yface<true>(-factor, wz0, wz1, tyy);
		
		_average_yface<false>(1, vz0, vz1, tyz);
		_average_yface<true>(1, wy0, wy1, tyz);
		
		static const int PITCHIN = InputSOA_ST::PITCH;
		static const int PITCHOUT = TempPiYSOA_ST::PITCH;
		
		const float * const mubase = _mu.ptr(0,0); 
		float * const tyxbase = &tyx.ref(0,0); 
		float * const tyybase = &tyy.ref(0,0); 
		float * const tyzbase = &tyz.ref(0,0); 
		
		for(int iy=0; iy<TempPiYSOA_ST::NY; iy++)
		{
			const float * const muptr = mubase + iy*PITCHIN; 
			float * const tyxptr = tyxbase + iy*PITCHOUT;
			float * const tyyptr = tyybase + iy*PITCHOUT;
			float * const tyzptr = tyzbase + iy*PITCHOUT;
			
			for(int ix=0; ix<TempPiYSOA_ST::NX; ix+=8)
			{
				const __m256 mu = _mm256_load_ps(muptr + ix - PITCHIN) + _mm256_load_ps(muptr + ix);
				
				_mm256_store_ps(tyxptr + ix, mu*_mm256_load_ps(tyxptr + ix));
				_mm256_store_ps(tyyptr + ix, mu*_mm256_load_ps(tyyptr + ix));
				_mm256_store_ps(tyzptr + ix, mu*_mm256_load_ps(tyzptr + ix));
			}
		}
#endif
	}
	

	void _zface(const InputSOAf_ST& mu0, const InputSOAf_ST& mu1,
							   const TempSOAf_ST& ux, const TempSOAf_ST& uz, 
							   const TempSOAf_ST& vy, const TempSOAf_ST& vz, 
							   const TempSOAf_ST& wx, const TempSOAf_ST& wy, const TempSOAf_ST& wz,
							   TempPiZSOAf_ST& tzx, TempPiZSOAf_ST& tzy, TempPiZSOAf_ST& tzz)
	{	
#ifndef _SP_COMP_
		printf("Diffusion_AVX::_zface: you should not be here in double precision. Aborting.\n");
		abort();
#else
		const float factor = (float)(2./3);
				
		_average_zface<false>(1, uz, tzx);
		_average_zface<true>(1, wx, tzx);
		
		_average_zface<false>(1, vz, tzy);
		_average_zface<true>(1, wy, tzy);
		
		_average_zface<false>(-factor, ux, tzz);
		_average_zface<true>(-factor, vy, tzz);
		_average_zface<true>(2-factor, wz, tzz);		
		
		static const int PITCHIN = InputSOA_ST::PITCH;
		static const int PITCHOUT = TempPiZSOA_ST::PITCH;
		
		const float * const mubase0 = mu0.ptr(0,0); 
		const float * const mubase1 = mu1.ptr(0,0); 
		float * const tzxbase = &tzx.ref(0,0); 
		float * const tzybase = &tzy.ref(0,0); 
		float * const tzzbase = &tzz.ref(0,0); 
		
		static const int STRIPES = 4;

		for(int iy=0; iy<TempPiZSOA_ST::NY; iy+=STRIPES)
		{
			float __attribute__((aligned(_ALIGNBYTES_))) mus[STRIPES*PITCHOUT];

			{
				const float * const muptr0 = mubase0 + iy*PITCHIN; 
				const float * const muptr1 = mubase1 + iy*PITCHIN; 
				for(int s=0; s<STRIPES; ++s)
					for(int ix=0; ix<TempPiZSOA_ST::NX; ix+=8)
						_mm256_store_ps(mus + ix + s*PITCHOUT, _mm256_load_ps(muptr0 + ix + s*PITCHIN) + _mm256_load_ps(muptr1 + ix + s*PITCHIN));
			}
			
			float * const tzxptr = tzxbase + iy*PITCHOUT;
			for(int s=0; s<STRIPES; ++s)
				for(int ix=0; ix<TempPiZSOA_ST::NX; ix+=8)
					_mm256_store_ps(tzxptr + ix + s*PITCHOUT,  _mm256_load_ps(mus + ix + s*PITCHOUT)*_mm256_load_ps(tzxptr + ix + s*PITCHOUT));

			float * const tzyptr = tzybase + iy*PITCHOUT;
			for(int s=0; s<STRIPES; ++s)
				for(int ix=0; ix<TempPiZSOA_ST::NX; ix+=8)
					_mm256_store_ps(tzyptr + ix + s*PITCHOUT,  _mm256_load_ps(mus + ix + s*PITCHOUT)*_mm256_load_ps(tzyptr + ix + s*PITCHOUT));

			float * const tzzptr = tzzbase + iy*PITCHOUT;
			for(int s=0; s<STRIPES; ++s)
				for(int ix=0; ix<TempPiZSOA_ST::NX; ix+=8)
					_mm256_store_ps(tzzptr + ix + s*PITCHOUT,  _mm256_load_ps(mus + ix + s*PITCHOUT)*_mm256_load_ps(tzzptr + ix + s*PITCHOUT));
		}
#endif
	}
};