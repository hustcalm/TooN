// -*- c++ -*-

//     Copyright (C) 2002 Tom Drummond (twd20@eng.cam.ac.uk)

//     This library is free software; you can redistribute it and/or
//     modify it under the terms of the GNU Lesser General Public
//     License as published by the Free Software Foundation; either
//     version 2.1 of the License, or (at your option) any later version.

//     This library is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//     Lesser General Public License for more details.

//     You should have received a copy of the GNU Lesser General Public
//     License along with this library; if not, write to the Free Software
//     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#ifndef CHOLESKY_H
#define CHOLESKY_H

// #include <TooN/lapack.h>

#include <TooN/TooN.h>
#include <TooN/helpers.h>
#include <limits>

#ifndef TOON_NO_NAMESPACE
namespace TooN {
#endif 



	// Tom's attempt using the non-sqrt version of the decomposition
	// symmetric M = L*D*L.T()
template <int Size, class Precision=double>
class Cholesky {
public:
	Cholesky(){}

	template<class P2, class B2>
	Cholesky(const Matrix<Size, Size, P2, B2>& m)
		: my_cholesky(m) {
		SizeMismatch<Size,Size>::test(m.num_rows(), m.num_cols());
		compute(m);
	}
	
	// for Size=Dynamic
	Cholesky(int size) : my_cholesky(size,size) {}


	template<class P2, class B2> void compute(const Matrix<Size, Size, P2, B2>& m){
		SizeMismatch<Size,Size>::test(m.num_rows(), m.num_cols());
		SizeMismatch<Size,Size>::test(m.num_rows(), my_cholesky.num_rows());
		my_cholesky=m;
		int size=my_cholesky.num_rows();
		for(int col=0; col<size; col++){
			Precision inv_diag=1/my_cholesky(col,col);
			for(int row=col; row < size; row++){
				// correct for the parts of cholesky already computed
				Precision val = my_cholesky(row,col);
				for(int col2=0; col2<col; col2++){
					val-=my_cholesky(col,col2)*my_cholesky(row,col2)*my_cholesky(col2,col2);
				}
				if(row==col){
					// this is the diagonal element so don't divide
					my_cholesky(row,col)=val;
				} else {
					// divide my the diagonal element for all others
					my_cholesky(row,col)=val*inv_diag;
				}
			}
		}
	}

	template<int Size2, class P2, class B2>
	Vector<Size, Precision> backsub (const Vector<Size2, P2, B2>& v) {
		int size=my_cholesky.num_rows();
		SizeMismatch<Size,Size2>::test(size, v.size());

		// first backsub through L
		Vector<Size, Precision> y(size);
		for(int i=0; i<size; i++){
			Precision val = v[i];
			for(int j=0; j<i; j++){
				val -= my_cholesky(i,j)*y[j];
			}
			y[i]=val;
		}
		
		// backsub through diagonal
		for(int i=0; i<size; i++){
			y[i]/=my_cholesky(i,i);
		}

		// backsub through L.T()
		Vector<Size,Precision> result(size);
		for(int i=size-1; i>=0; i--){
			Precision val = y[i];
			for(int j=i+1; j<size; j++){
				val -= my_cholesky(j,i)*result[j];
			}
			result[i]=val;
		}

		return result;
	}

	Matrix<Size,Size,Precision> my_cholesky;
};


#if 0  // should be removed and possibly replaced with wls_cholesky implementation for small fixed sizes
    namespace util {
	
	template <int B, int E> struct Dot3 {
	    template <class V1, class V2, class V3> static inline double eval(const V1& a, const V2& b, const V3& c) { return a[B]*b[B]*c[B] + Dot3<B+1,E>::eval(a,b,c); }
	};
	template <int B> struct Dot3<B,B> {
	    template <class V1, class V2, class V3> static inline double eval(const V1& a, const V2& b, const V3& c) { return a[B]*b[B]*c[B]; }
	};

	//
	// Forward Sub
	//
	template <int N, int I=0> struct Forwardsub_L {
	    template <class A1, class A2, class A3> static inline void eval(const FixedMatrix<N,N,A1>& L, const FixedVector<N,A2>& v, FixedVector<N,A3>& x) {
		x[I] = v[I] - Dot<0,I-1>::eval(L[I], x);
		Forwardsub_L<N,I+1>::eval(L, v, x);
	    }
	    template <class A1, class A2, class Vec> static inline void eval(const FixedMatrix<N,N,A1>& L, const DynamicVector<A2>& v, Vec & x) {
		x[I] = v[I] - Dot<0,I-1>::eval(L[I], x);
		Forwardsub_L<N,I+1>::eval(L, v, x);
	    }
	};
	template <int N> struct Forwardsub_L<N,0> {
	    template <class A1, class A2, class A3> static inline void eval(const FixedMatrix<N,N,A1>& L, const FixedVector<N,A2>& v, FixedVector<N,A3>& x) {
		x[0] = v[0];
		Forwardsub_L<N,1>::eval(L, v, x);
	    }
	    template <class A1, class A2, class Vec> static inline void eval(const FixedMatrix<N,N,A1>& L, const DynamicVector<A2>& v, Vec & x) {
		assert(v.size() == N && x.size() == N);
		x[0] = v[0];
		Forwardsub_L<N,1>::eval(L, v, x);
	    }
	};
	template <int N> struct Forwardsub_L<N,N> {
	    template <class A1, class A2, class A3> static inline void eval(const FixedMatrix<N,N,A1>& /*L*/, const FixedVector<N,A2>& /*v*/, FixedVector<N,A3>& /*x*/) {}
	    template <class A1, class A2, class Vec> static inline void eval(const FixedMatrix<N,N,A1>& /*L*/, const DynamicVector<A2>& /*v*/, Vec & /*x*/) {}
	};
	
	//
	// Back Sub
	//
	template <int N, int I=N> struct Backsub_LT {
	    template <class A1, class A2, class A3, class A4> static inline void eval(const FixedMatrix<N,N,A1>& L, const FixedVector<N,A2>& v, 
										      const FixedVector<N,A3>& invdiag, FixedVector<N,A4>& x) {
		x[I-1] = v[I-1]*invdiag[I-1] - Dot<I,N-1>::eval(L.T()[I-1], x);
		Backsub_LT<N,I-1>::eval(L, v, invdiag, x);
	    }
	};
	template <int N> struct Backsub_LT<N,N> {
	    template <class A1, class A2, class A3, class A4> static inline void eval(const FixedMatrix<N,N,A1>& L, const FixedVector<N,A2>& v, 
										      const FixedVector<N,A3>& invdiag, FixedVector<N,A4>& x) {
		x[N-1] = v[N-1]*invdiag[N-1];
		Backsub_LT<N,N-1>::eval(L, v, invdiag, x);
	    }
	};
	template <int N> struct Backsub_LT<N,0> {
	    template <class A1, class A2, class A3, class A4> static inline void eval(const FixedMatrix<N,N,A1>& /*L*/, const FixedVector<N,A2>& /*v*/, 
										      const FixedVector<N,A3>& /*invdiag*/, FixedVector<N,A4>& /*x*/) {}
	};

	template <int N, class A1, class A2, class A3, class A4>
	void cholesky_solve(const FixedMatrix<N,N,A1>& L, const FixedVector<N,A2>& invdiag, const FixedVector<N,A3>& v, FixedVector<N,A4>& x) 
	{
	    Vector<N> t;
	    Forwardsub_L<N>::eval(L, v, t);
	    Backsub_LT<N>::eval(L, t, invdiag, x);
	}

	//
	// Compute cholesky
	//
	template <int N, int I, int J=I+1> struct CholeskyInner {
	    template <class A1, class A2, class A3> static inline void eval(const FixedMatrix<N,N,A1>& M, FixedMatrix<N,N,A2>& L, const FixedVector<N,A3>& invdiag) {
		const double a = M[I][J] - Dot<0,I-1>::eval(L[I], L.T()[J]);
		L[I][J] = a;
		L[J][I] = a * invdiag[I];
		CholeskyInner<N, I, J+1>::eval(M, L, invdiag);
	    }
	};

	template <int N, int I> struct CholeskyInner<N,I,N> {
	    template <class A1, class A2, class A3> static inline void eval(const FixedMatrix<N,N,A1>& /*M*/, FixedMatrix<N,N,A2>& /*L*/, const FixedVector<N,A3>& /*invdiag*/) {}
	};
	template <int N, int I=0> struct CholeskyOuter {
	    template <class A1, class A2, class A3> static inline void eval(const FixedMatrix<N,N,A1>& M, FixedMatrix<N,N,A2>& L, FixedVector<N,A3>& invdiag, int& rank) {
		const double a = M[I][I] - Dot<0,I-1>::eval(L[I], L.T()[I]);
		if (0 < a) {
		    L[I][I] = a;
		    invdiag[I] = 1.0/a;
		    ++rank;
		} else {
		    L[I][I] = invdiag[I] = 0;
		    return;
		}
		CholeskyInner<N,I>::eval(M,L,invdiag);
		CholeskyOuter<N,I+1>::eval(M,L,invdiag,rank);
	    }
	};
	template <int N> struct CholeskyOuter<N,N> {
	    template <class A1, class A2, class A3> static inline void eval(const FixedMatrix<N,N,A1>& /*M*/, FixedMatrix<N,N,A2>& /*L*/, FixedVector<N,A3>& /*invdiag*/, int& /*rank*/) {}
	};


	template <int N, class A1, class A2, class A3> int cholesky_compute(const FixedMatrix<N,N,A1>& M, FixedMatrix<N,N,A2>& L, FixedVector<N,A3>& invdiag)
	{
	    int rank=0;
	    CholeskyOuter<N>::eval(M, L, invdiag, rank);
	    return rank;
	}

	//
	// Compute inverse
	//
	template <int N, int Col, int I=Col+1> struct InverseForward {
	    template <class A1, class A2> static inline void eval(const FixedMatrix<N,N,A1>& L, FixedVector<N,A2>& t) {
		t[I] = -(L[I][Col] + Dot<Col+1,I-1>::eval(L[I], t));
		InverseForward<N,Col,I+1>::eval(L,t);
	    }
	};
	template <int N, int Col> struct InverseForward<N,Col,N> { 
	    template <class A1, class A2> static inline void eval(const FixedMatrix<N,N,A1>& L, FixedVector<N,A2>& t) {} 
	};

	template <int N, int Col, int I=N-1> struct InverseBack {
	    template <class A1, class A2, class A3, class A4> static inline void eval(const FixedMatrix<N,N,A1>& L, const FixedVector<N,A2>& t, 
										      const FixedVector<N,A3>& invdiag, FixedMatrix<N,N,A4>& Inv) {
		Inv[I][Col] = Inv[Col][I] = invdiag[I]*t[I] - Dot<I+1,N-1>::eval(L.T()[I], Inv[Col]);
		InverseBack<N,Col,I-1>::eval(L, t, invdiag, Inv);
	    }
	};
	template <int N, int Col> struct InverseBack<N,Col,Col> {
	    template <class A1, class A2, class A3, class A4> static inline void eval(const FixedMatrix<N,N,A1>& L, const FixedVector<N,A2>& t, 
										      const FixedVector<N,A3>& invdiag, FixedMatrix<N,N,A4>& Inv) {
		Inv[Col][Col] = invdiag[Col] - Dot<Col+1,N-1>::eval(L.T()[Col], Inv[Col]);
	    }
	};
	
	template <int N, int Col=0> struct InverseOuter {
	    template <class A1, class A2, class A3> static inline void eval(const FixedMatrix<N,N,A1>& L, const FixedVector<N,A2>& invdiag, FixedMatrix<N,N,A3>& Inv) {
		Vector<N> t;
		InverseForward<N,Col>::eval(L, t);
		InverseBack<N,Col>::eval(L, t, invdiag, Inv);
		InverseOuter<N,Col+1>::eval(L,invdiag,Inv);
	    }
	};
	template <int N> struct InverseOuter<N,N> {
	    template <class A1, class A2, class A3> static inline void eval(const FixedMatrix<N,N,A1>& L, const FixedVector<N,A2>& invdiag, FixedMatrix<N,N,A3>& Inv) {}
	};

	template <int S, class A1, class A2, class A3> void cholesky_inverse(const FixedMatrix<S,S,A1>& L, const FixedVector<S,A2>& invdiag, FixedMatrix<S,S,A3>& I)
	{
	    InverseOuter<S>::eval(L, invdiag, I);
	}

	//
	// Cholesky update
	//
	template <int N, int I, int J=I-1> struct UpdateInner {
	    template <class LT, class PT, class FT, class ST> static inline void eval(LT& L, const PT& p, const FT& F, const ST sigma) {
		const double old = L[I][J];
		L[I][J] = old + (sigma * p[J]) * F[J];
		UpdateInner<N,I,J-1>::eval(L, p, F, sigma + old * p[J]);
	    }
	};

	template <int N, int I> struct UpdateInner<N,I,-1> {
	    template <class LT, class PT, class FT, class ST> static inline void eval(LT& L, const PT& p, const FT& F, const ST sigma) {}
	};
	    
	template <int N, int I=1> struct UpdateOuter {
	    template <class LT, class PT, class FT> static inline void eval(LT& L, const PT& p, const FT& F) {
		UpdateInner<N,I>::eval(L, p, F, p[I]);
		UpdateOuter<N,I+1>::eval(L, p ,F);
	    }
	};

	template <int N> struct UpdateOuter<N,N> {
	    template <class LT, class PT, class FT> static inline void eval(LT& L, const PT& p, const FT& F) {}
	};
	    
	template <class P, int N, class A1, class A2> void cholesky_update(TooN::FixedMatrix<N,N,A1>& L, TooN::FixedVector<N,A2>& invdiag, const P& p)
	{
	    double F[N-1];
	    double alpha = 1;
	    for (int i=0; i<N-1; ++i) {
		const double p2 = p[i]*p[i];
		const double di = L[i][i];
		L[i][i] += alpha * p2;
		invdiag[i] = 1.0/L[i][i];
		const double a_over_d = alpha*invdiag[i];
		F[i] = a_over_d;
		alpha = di * a_over_d;
	    }
	    L[N-1][N-1] += alpha * (p[N-1]*p[N-1]);
	    invdiag[N-1] = 1.0/L[N-1][N-1];
		
	    UpdateOuter<N>::eval(L, p, F);
	}
    }

    template <int N=-1, typename Precision = double>
    class Cholesky {
    public:

		Cholesky() {}

		template<int R, int C, typename P, typename A>
		Cholesky(const Matrix<R,C,P,A>& m) : L(m.num_rows(), m.num_cols()), invdiag(m.num_cols()) {
			compute(m);
		}
    
		template<int R, int C, typename P, typename A>
		void compute(const Matrix<R,C,P,A>& m){
			SizeMismatch<R,C>::test(m.num_rows(), m.num_cols());
			SizeMismatch<R,N>::test(m.num_rows(), L.num_rows());
			// rank = util::cholesky_compute(m,L,invdiag);
			L = m;
			int Size = L.num_rows();
			int info;
			dpotrf_("L", &Size, L.get_data_ptr(), &Size, &info);
			assert(info >= 0);
			if (info > 0)
				rank = info-1;
		}
		int get_rank() const { return rank; }
		
		Precision get_determinant() const {
			Precision det = L(0,0);
			for (int i=1; i<L.num_rows(); ++i)
				det *= L(i,i);
			return det;
		}

		const Matrix<N,N,Precision> & get_L_D() const { return L; }

		template <class A1, class A2> static void sqrt(const FixedMatrix<N,N,A1>& A, FixedMatrix<N,N,A2>& L) {
			for (int i=0; i<N; ++i) {
				double a = A[i][i];
				for (int k=0; k<i; ++k)
					a -= L[i][k]*L[i][k];
				if (0<a) {
					L[i][i] = ::sqrt(a);
				} else {
					Zero(L.slice(i,i,N-i,N-i));
					return;
				}
				const double id = 1.0/L[i][i];
				for (int j=i+1; j<N; ++j) {
					L[i][j] = 0;
					double a = A[i][j];
					for (int k=0; k<i; ++k)
						a -= L[i][k]*L[j][k];
					L[j][i] = a * id;
				}
			}
		}
		
		template <class A1> static Matrix<N> sqrt(const FixedMatrix<N,N,A1>& A) { 
			Matrix<N> L;
			Cholesky<N>::sqrt(A,L);
			return L;
		}

		template <class A> void get_sqrt(FixedMatrix<N,N,A>& M) const {
			for (int i=0; i<N; ++i) {
				const double root_d = ::sqrt(L[i][i]);
				M[i][i] = root_d;
				for (int j=i+1; j<N; ++j) {
					M[j][i] = L[j][i]*root_d;
					M[i][j] = 0;
				}
			}
		}
		
		Matrix<N> get_sqrt() const {
			Matrix<N> S;
			get_sqrt(S);
			return S;
		}
	
		Matrix<N> get_L() const { return get_sqrt(); }
	
		template <class A> void get_inv_sqrt(FixedMatrix<N,N,A>& M) const {
			Vector<N> root;
			for (int i=0; i<N; ++i)
				root[i] = ::sqrt(invdiag[i]);
			for (int j=0; j<N; ++j) {
				for (int i=j+1; i<N; ++i) {
					double sum = L[i][j];
					for (int k=j+1; k<i; ++k)
						sum += L[i][k]*M[j][k];
					M[j][i] = -sum;
					M[i][j] = 0;
				}
				M[j][j] = root[j];
				for (int i=j+1; i<N; ++i)
					M[j][i] *= root[i];
			}
		}
	
		template <class A> double mahalanobis(const FixedVector<N,A>& v) const {
			Vector<N> L_inv_v;
			util::Forwardsub_L<N>::eval(L, v, L_inv_v);
			return util::Dot3<0,N-1>::eval(L_inv_v, L_inv_v, invdiag);
		}
		
		template <class F, int M, class A1, class A2> void transform_inverse(const FixedMatrix<M,N,A1>& J, FixedMatrix<M,M,A2>& T) const {
			Matrix<M,N> L_inv_JT;
			for (int i=0; i<M; ++i)
				util::Forwardsub_L<N>::eval(L, J[i], L_inv_JT[i]);
			for (int i=0; i<M; ++i) {		
				F::eval(T[i][i],util::Dot3<0,N-1>::eval(L_inv_JT[i], L_inv_JT[i], invdiag));
				for (int j=i+1; j<M; ++j) {
					const double x = util::Dot3<0,N-1>::eval(L_inv_JT[i], L_inv_JT[j], invdiag);
					F::eval(T[i][j],x);
					F::eval(T[j][i],x);
				}
			}
		}
		
		template <class F, class A1, class M2> void transform_inverse(const DynamicMatrix<A1>& J, M2 & T) const {
			const int M = J.num_rows();
			assert( T.num_rows() == M && T.num_cols() == M);
			Matrix<> L_inv_JT(M,N);
			for (int i=0; i<M; ++i)
				util::Forwardsub_L<N>::eval(L, J[i], L_inv_JT[i]);
			for (int i=0; i<M; ++i) {
				F::eval(T[i][i],util::Dot3<0,N-1>::eval(L_inv_JT[i], L_inv_JT[i], invdiag));
				for (int j=i+1; j<M; ++j) {
					const double x = util::Dot3<0,N-1>::eval(L_inv_JT[i], L_inv_JT[j], invdiag);
					F::eval(T[i][j],x);
					F::eval(T[j][i],x);
				}
			}
		}
		
		template <int M, class A1, class A2> void transform_inverse(const FixedMatrix<M,N,A1>& J, FixedMatrix<M,M,A2>& T) const {
			transform_inverse<util::Assign>(J,T);
		}

		template <class A1, class M2> void transform_inverse(const DynamicMatrix<A1>& J, M2 & T) const {
			transform_inverse<util::Assign>(J,T);
		}
		
		template <int M, class A> Matrix<M> transform_inverse(const FixedMatrix<M,N,A>& J) const {
			Matrix<M> T;
			transform_inverse(J,T);
			return T;
		}
		
		template <class A> Matrix<> transform_inverse(const DynamicMatrix<A>& J) const {
			Matrix<> T(J.num_rows(), J.num_rows());
			transform_inverse(J,T);
			return T;
		}

		template <class A1, class A2> inline 
		void inverse_times(const FixedVector<N,A1>& v, FixedVector<N,A2>& x) const
		{
			util::cholesky_solve(L, invdiag, v, x);
		}
		
		template <class Accessor> inline 
		Vector<N> inverse_times(const FixedVector<N,Accessor>& v) const
		{
			Vector<N> x;
			inverse_times(v, x);
			return x;
		}
		
		template <class Accessor> inline 
		Vector<N> backsub(const FixedVector<N,Accessor>& v) const { return inverse_times(v); }
		
		template <class A, int M> inline Matrix<N,M> inverse_times(const FixedMatrix<N,M,A>& m)
		{
			Matrix<N,M> result;
			for (int i=0; i<M; i++)
				inverse_times(m.T()[i], result.T()[i]);
			return result;
		}

		template <class A> void get_inverse(FixedMatrix<N,N,A>& M) const {
			util::cholesky_inverse(L, invdiag, M);
		}
		
		Matrix<N> get_inverse() const {
			Matrix<N> M;
			get_inverse(M);
			return M;
		}
	
		template <int M, class A>  void update(const FixedMatrix<N,M,A>& V) {
			for (int i=0; i<M; ++i) {
				Vector<N> p;
				util::Forwardsub_L<N>::eval(L, V.T()[i], p);
				util::cholesky_update(L, invdiag, p);
			}
		}
		
		template <class A>  void update(const FixedVector<N,A>& v) {
			Vector<N> p;
			util::Forwardsub_L<N>::eval(L, v, p);
			util::cholesky_update(L, invdiag, p);
		}
	
    private:
		Matrix<N, N, Precision> L;
		Vector<N, Precision> invdiag;
		int rank;
    };
  
	// #endif

template <int Size = -1, typename Precision = double>
class Cholesky {
public:

	Cholesky(){}

	template<int R, int C, typename P, typename B>
	Cholesky(const Matrix<R,C,P,B> & m)  : L(m.num_rows(), m.num_cols()) {
		compute(m);
	}

	template<int R, int C, typename P, typename B>
	void compute(const Matrix<R,C,P,B> & m){
		SizeMismatch<R,C>::test(m.num_rows(), m.num_cols());
		SizeMismatch<R,Size>::test(m.num_rows(), L.num_rows());
		L = m;
		int N = L.num_rows();
		potrf_("L", &N, L.my_data, &N, &info);
	}

	int get_info() const { return info; }

	template <int S, typename P, typename B>
	Vector<Size, Precision> inverse_times(const Vector<S,P,B> & v) const { return backsub(v); }

	template <int S, typename P, typename B>
	Vector<Size, Precision> backsub(const Vector<S,P,B> & v) const
	{
		SizeMismatch<S,Size>::test(v.size(), L.num_rows());
		Vector<Size, Precision> x = v;
		int N=L.num_rows();
		int NRHS=1;
		potrs_("L", &N, &NRHS, L.my_data, &N, x.my_data, &N, &info);	    
		return x;
	}

	template <int S, typename P, typename B>
	Precision mahalanobis(const Vector<S,P,B> & v) const {
		return v * backsub(v);
	}

	const Matrix<Size,Size,Precision>& get_L() const {
		return L;
	}

	Precision get_determinant() const {
		Precision det = L(0,0);
		for (int i=1; i<L.num_rows(); ++i)
			det *= L(i,i);
		return det*det;
	}

	Matrix<Size, Size, Precision> get_inverse() const {
		Matrix<Size, Size, Precision> M = L;
		int N = L.num_rows();
		potri_("L", &N, M.my_data, &N, &info);
		for(int i = 1; i < M.num_rows(); ++i)
			for(int j = 0; j < i; ++j)
				M(i,j) = M(j,i);
		return M;
	}

private:
	Matrix<Size, Size, Precision> L;
	mutable int info;
};

#endif

#ifndef TOON_NO_NAMESPACE
}
#endif 

#endif
