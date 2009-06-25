// -*- c++ -*-

// Copyright (C) 2005,2009 Tom Drummond (twd20@cam.ac.uk),
// Ed Rosten (er258@cam.ac.uk), Gerhard Reitmayr (gr281@cam.ac.uk)
//
// This file is part of the TooN Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.


#ifndef TOON_INCLUDE_HELPERS_H
#define TOON_INCLUDE_HELPERS_H

#include <TooN/TooN.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#define M_SQRT1_2 0.70710678118654752440
#endif

namespace TooN {

	///\deprecated
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> TOON_DEPRECATED void Fill(Vector<Size, Precision, Base>& v, const Precision& p)
	{
		for(int i=0; i < v.size(); i++)
			v[i]= p;
	}

	///\deprecated
	///@ingroup gLinAlg
	template<int Rows, int Cols, class Precision, class Base> TOON_DEPRECATED void Fill(Matrix<Rows, Cols, Precision, Base>& m, const Precision& p)
	{
		for(int i=0; i < m.num_rows(); i++)
			for(int j=0; j < m.num_cols(); j++)
				m[i][j] = p;
	}
	
	///Compute the \f$L_2\f$ norm of \e v
	///@param v \e v
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> inline Precision norm(const Vector<Size, Precision, Base>& v)
	{
		using std::sqrt;
		return sqrt(v*v);
	}

	///Compute the \f$L_2^2\f$ norm of \e v
	///@param v \e v
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> inline Precision norm_sq(const Vector<Size, Precision, Base>& v)
	{
		return v*v;
	}


	///Compute a the unit vector \f$\hat{v}\f$.
	///@param v \e v
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> inline Vector<Size, Precision> unit(const Vector<Size, Precision, Base> & v)
	{
		using std::sqrt;
		return v * (1/sqrt(v*v));
	}
	
	//Note because of the overload later, this function will ONLY receive sliced vectors. Therefore
	//a copy can be made, which is still a slice, so operating on the copy operates on the original
	//data.
	///Normalize a vector in place
	///@param v Vector to normalize
	///@ingroup gLinAlg
	template<int Size, class Precision, class Base> inline void normalize(Vector<Size, Precision, Base> v)
	{
		using std::sqrt;
		v /= sqrt(v*v);
	}
	
	//This overload is required to operate on non-slice vectors
	/**
		\overload
	*/  
	template<int Size, class Precision> inline void normalize(Vector<Size, Precision> & v)
	{
		normalize(v.as_slice());
	}

	///For a vector \e v of length \e i, return \f$[v_1, v_2, \cdots, v_{i-1}] / v_i \f$
	///@param v \e v
	///@ingroup gLinAlg
	template<int Size, typename Precision, typename Base> inline Vector<Size-1, Precision> project( const Vector<Size, Precision, Base> & v){
		return v.template slice<0,Size-1>() / v[Size-1];
	}
	
	/**
	  \overload
	*/
	template<typename Precision, typename Base> inline Vector<-1, Precision> project( const Vector<-1, Precision, Base> & v){
		return v.slice(0,v.size()-1) / v[v.size()-1];
	}
	
	///For a vector \e v of length \e i, return \f$[v_1, v_2, \cdots, v_{i}, 1]\f$
	///@param v \e v
	///@ingroup gLinAlg
	template<int Size, typename Precision, typename Base> inline Vector<Size+1, Precision> unproject( const Vector<Size, Precision, Base> & v){
		Vector<Size+1, Precision> result;
		result.template slice<0,Size>() = v;
		result[Size] = 1;
		return result;
	}
	
	/**
	  \overload
	*/
	template<typename Precision, typename Base> inline Vector<-1, Precision> unproject( const Vector<-1, Precision, Base> & v){
		Vector<-1, Precision> result(v.size()+1);
		result.slice(0,v.size()) = v;
		result[v.size()] = 1;
		return result;
	}

	/**
	  \overload
	*/
	template<int R, int C, typename Precision, typename Base> inline Matrix<R-1, C, Precision> project( const Matrix<R,C, Precision, Base> & m){
       Matrix<R-1, C, Precision> result = m.template slice<0,0,R-1,C>();
       for( int c = 0; c < C; ++c ) {
           result.slice(0,c,R-1,1) /= m[R-1][c];
       }
       return result;
   }

	/**
	  \overload
	*/
   template<int R, int C, typename Precision, typename Base> inline Matrix<R+1, C, Precision> unproject( const Matrix<R, C, Precision, Base> & m){
       Matrix<R+1, C, Precision> result;
       result.template slice<0,0,R,C>() = m;
	   result[R] = Ones;
       return result;
   }



	/// Frobenius (root of sum of squares) norm of input matrix \e m
	///@param m \e m
	///@ingroup gLinAlg
	template <int R, int C, typename P, typename B>
	P inline norm_fro( const Matrix<R,C,P,B> & m ){
		using std::sqrt;
		P n = 0;
		for(int r = 0; r < m.num_rows(); ++r)
			for(int c = 0; c < m.num_cols(); ++c)
				n += m[r][c] * m[r][c];

		return sqrt(n);
	}

	/// \e L<sub>&#8734;</sub> (row sum) norm of input matrix m
	/// computes the maximum of the sums of absolute values over rows
	///@ingroup gLinAlg
	template <int R, int C, typename P, typename B>
	P inline norm_inf( const Matrix<R,C,P,B> & m ){
		using std::abs;
		using std::max;
		P n = 0;
		for(int r = 0; r < m.num_rows(); ++r){
			P s = 0;
			for(int c = 0; c < m.num_cols(); ++c)
				s += abs(m(r,c));
			n = max(n,s);
		}
		return n;
	}
	
	/// \e L<sub>1</sub> (col sum) norm of input matrix m
	/// computes the maximum of the sums of absolute values over columns
	///@ingroup gLinAlg
	template <int R, int C, typename P, typename B>
	P inline norm_1( const Matrix<R,C,P,B> & m ){
		using std::abs;
		using std::max;
		P n = 0;
		for(int c = 0; c < m.num_cols(); ++c){
			P s = 0;
			for(int r = 0; r < m.num_rows(); ++r)
				s += abs(m(r,c));
			n = max(n,s);
		}
		return n;
	}

	namespace Internal {
		template <int R, int C, typename P, typename B>
		inline Matrix<R, C, P> exp_taylor( const Matrix<R,C,P,B> & m ){
			TooN::SizeMismatch<R, C>::test(m.num_rows(), m.num_cols());
			Matrix<R,C,P> result = TooN::Zeros(m.num_rows(), m.num_cols());
			Matrix<R,C,P> f = TooN::Identity(m.num_rows());
			P k = 1;
			while(norm_inf((result+f)-result) > 0){
				result += f;
				f = (m * f) / k;
				k += 1;
			}
			return result;
		}
	};
	
	/// computes the matrix exponential of a matrix m by 
	/// scaling m by 1/(powers of 2), using Taylor series and 
	/// squaring again.
	/// @param m input matrix, must be square
	/// @return result matrix of the same size/type as input
	/// @ingroup gLinAlg
	template <int R, int C, typename P, typename B>
	inline Matrix<R, C, P> exp( const Matrix<R,C,P,B> & m ){
		using std::max;
		SizeMismatch<R, C>::test(m.num_rows(), m.num_cols());
		const P l = log2(norm_inf(m));
		const int s = max(0,(int)ceil(l));
		Matrix<R,C,P> result = Internal::exp_taylor(m/(1<<s));
		for(int i = 0; i < s; ++i)
			result = result * result;
		return result;
	}
	
	/// Returns true if every element is finite
	///@ingroup gLinAlg
	template<int S, class P, class B> bool isfinite(const Vector<S, P, B>& v)
	{ 
		using std::isfinite;
		for(int i=0; i < v.size(); i++)
			if(!isfinite(v[i]))
				return 0;
		return 1;
	}

	/// Returns true if any element is NaN
	///@ingroup gLinAlg
	template<int S, class P, class B> bool isnan(const Vector<S, P, B>& v)
	{ 
		using std::isnan;
		for(int i=0; i < v.size(); i++)
			if(isnan(v[i]))
				return 1;
		return 0;
	}

	/// Symmetrize a matrix 
	///@param m \e m
	///@return \f$ \frac{m + m^{\mathsf T}}{2} \f$
	///@ingroup gLinAlg
	template<int Rows, int Cols, typename Precision, typename Base>
	void Symmetrize(Matrix<Rows,Cols,Precision,Base>& m){
		SizeMismatch<Rows,Cols>::test(m.num_rows(), m.num_cols());
		for(int r=0; r<m.num_rows()-1; r++){
			for(int c=r+1; c<m.num_cols(); c++){
				const Precision temp=(m(r,c)+m(c,r))/2;
				m(r,c)=temp;
				m(c,r)=temp;
			}
		}
	}

	
	/// computes the trace of a square matrix
	///@ingroup gLinAlg
	template<int Rows, int Cols, typename Precision, typename Base>
	Precision trace(const Matrix<Rows, Cols, Precision, Base> & m ){
		SizeMismatch<Rows,Cols>::test(m.num_rows(), m.num_cols());
		Precision tr = 0;
		for(int i = 0; i < m.num_rows(); ++i)
			tr += m(i,i);
		return tr;
	}

	/// creates an returns a cross product matrix M from a 3 vector v, such that for all vectors w, the following holds: v ^ w = M * w
	/// @param vec the 3 vector input
	/// @return the 3x3 matrix to set to the cross product matrix
	///@ingroup gLinAlg
	template<int Size, class P, class B> inline TooN::Matrix<3, 3, P> cross_product_matrix(const Vector<Size, P, B>& vec)
	{
		SizeMismatch<Size,3>::test(vec.size(), 3);

		TooN::Matrix<3> result;

		result(0,0) = 0; 
		result(0,1) = -vec[2]; 
		result(0,2) = vec[1];
		result(1,0) = vec[2]; 
		result(1,1) = 0; 
		result(1,2) = -vec[0];
		result(2,0) = -vec[1]; 
		result(2,1) = vec[0]; 
		result(2,2) = 0;

		return result;
	}

    namespace Internal {

        template<int Size, typename Precision, typename Base, typename Func, typename Ret> inline Ret accumulate( const Vector<Size, Precision, Base> & v, Func func )  {
            if( v.size() == 0 ) {
                return 0; // What should we return, exception?
            }
            func.initialise( v[0], 0 );
            for( int ii = 1; ii < v.size(); ii++ ) {
                func( v[ii], ii );
            }
            return func.ret();
        }

        template<int R, int C, typename Precision, typename Base, typename Func, typename Ret> inline Ret accumulate( const Matrix<R, C, Precision, Base> & m, Func func )  {
            if( m.num_rows() == 0 || m.num_cols() == 0) {
                return 0; // What should we return, exception?
            }
            func.initialise( m[0][0], 0, 0 );
            for(int r=0; r<m.num_rows(); r++){
                for(int c=0; c<m.num_cols(); c++){
                    func( m[r][c], r, c );
                }
            }
            return func.ret();
        }
        template<int R, int C, typename Precision, typename Base, typename Func, typename Ret> inline Ret& accumulate_horizontal( const Matrix<R, C, Precision, Base> & m, Func func ) {
            if( m.num_cols() == 0 || m.num_rows() == 0 ) {
                func.null(); // What should we return, exception?
            }
            for(int r=0; r<m.num_rows(); r++){
                func.initialise( m[r][0], r, 0 );
                for(int c=1; c<m.num_cols(); c++){
                    func( m[r][c], r, c );
                }
            }
            return func.ret();
        }
        template<int R, int C, typename Precision, typename Base, typename Func, typename Ret> inline Ret& accumulate_vertical( const Matrix<R, C, Precision, Base> & m, Func func ) {
            if( m.num_cols() == 0 || m.num_rows() == 0 ) {
                func.null(); // What should we return, exception?
            }
            for(int c=0; c<m.num_cols(); c++){
                func.initialise( m[0][c], 0, c );
                for(int r=1; r<m.num_rows(); r++){
                    func( m[r][c], r, c );
                }
            }
            return func.ret();
        }        

        template<class Precision>
        class comp_less {
        public:
            bool operator()( Precision A, Precision B ) {
                return A < B;
            }
        };
        template<class Precision>
        class comp_greater {
        public:
            bool operator()( Precision A, Precision B ) {
                return A > B;
            }
        };

        template<typename Precision, typename ComparisonFunctor>
        class accumulate_functor_vector {
            Precision bestVal;
        public:
            void initialise( Precision initialVal, int nIndex ) {
                bestVal = initialVal;
            }
            void operator()( Precision curVal, int nIndex ) {
                if( ComparisonFunctor()( curVal, bestVal ) ) {
                    bestVal = curVal;
                }
            }
            Precision ret() {
                return bestVal;
            }            
        };
        template<typename Precision, typename ComparisonFunctor>
        class accumulate_functor_matrix {
            Precision bestVal;
        public:
            void initialise( Precision initialVal, int nRow, int nCol ) {
                bestVal = initialVal;
            }
            void operator()( Precision curVal, int nRow, int nCol ) {
                if( ComparisonFunctor()( curVal, bestVal ) ) {
                    bestVal = curVal;
                }
            }
            Precision ret() {
                return bestVal;
            }            
        };
        template<typename Precision, typename ComparisonFunctor>
        class accumulate_vertical_functor {
            Vector<>& bestVal;
        public:
            accumulate_vertical_functor( int nNumCols ) :
                bestVal( *(new Vector<>( nNumCols )) )
            {}
            Vector<>& null() {
                return bestVal;
            }
            void initialise( Precision initialVal, int nRow, int nCol ) {
                bestVal[nCol] = initialVal;
            }
            void operator()( Precision curVal, int nRow, int nCol ) {
                if( ComparisonFunctor()( curVal, bestVal[nCol] ) ) {
                    bestVal[nCol] = curVal;
                }
            }
            Vector<>& ret() {
                return bestVal;
            }            
        };
        template<typename Precision, typename ComparisonFunctor>
        class accumulate_horizontal_functor {
            Vector<>& bestVal;
        public:
            accumulate_horizontal_functor( int nNumRows ) :
                bestVal( *(new Vector<>( nNumRows )) )
            {}
            Vector<>& null() {
                return bestVal;
            }
            void initialise( Precision initialVal, int nRow, int nCol ) {
                bestVal[nRow] = initialVal;
            }
            void operator()( Precision curVal, int nRow, int nCol ) {
                if( ComparisonFunctor()( curVal, bestVal[nRow] ) ) {
                    bestVal[nRow] = curVal;
                }
            }
            Vector<>& ret() {
                return bestVal;
            }            
        };

        template<typename Precision, typename Index, typename ComparisonFunctor>
        class accumulate_element_functor {
            Precision bestVal;
            Index bestIndex;
        public:
            void initialise( Precision initialVal, Index index ) {
                bestVal = initialVal;
                bestIndex = index;
            }
            void operator()( Precision curVal, Index index ) {
                if( ComparisonFunctor()( bestVal, curVal ) ) {
                    bestVal = curVal;
                    bestIndex = index;
                }
            }
            std::pair<Precision,Index> ret() {
                return std::pair<Precision,Index>( bestVal, bestIndex );
            }            
        };
    }

    template<int Size, typename Precision, typename Base> inline Precision min( const Vector<Size, Precision, Base>& v) {
        typedef Internal::accumulate_functor_vector<Precision, Internal::comp_less<Precision> > vector_accumulate_functor;
        vector_accumulate_functor min_functor_inst;
        return Internal::accumulate<Size,Precision,Base,
            vector_accumulate_functor, Precision >( v, min_functor_inst ); 
    }
    template<int Size, typename Precision, typename Base> inline Precision max( const Vector<Size, Precision, Base>& v) {
        typedef Internal::accumulate_functor_vector<Precision, Internal::comp_greater<Precision> > vector_accumulate_functor;
        vector_accumulate_functor max_functor_inst;
        return Internal::accumulate<Size,Precision,Base,
            vector_accumulate_functor, Precision >( v, max_functor_inst ); 
    }

    template<int R, int C, typename Precision, typename Base> inline Precision min( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_functor_matrix<Precision, Internal::comp_less<Precision> > matrix_accumulate_functor;
        matrix_accumulate_functor min_functor_inst;
        return Internal::accumulate<R,C,Precision,Base,
            matrix_accumulate_functor, Precision>( m, min_functor_inst );
    }

    template<int R, int C, typename Precision, typename Base> inline Precision max( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_functor_matrix<Precision, Internal::comp_greater<Precision> > matrix_accumulate_functor;
        matrix_accumulate_functor max_functor_inst;
        return Internal::accumulate<R,C,Precision,Base,
            matrix_accumulate_functor, Precision>( m, max_functor_inst );
    }
    template<int R, int C, typename Precision, typename Base> inline Vector<>& min_vertical( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_vertical_functor<Precision,Internal::comp_less<Precision> > matrix_accumulate_vertical_functor;
        matrix_accumulate_vertical_functor min_functor_inst( m.num_cols() );
        return Internal::accumulate_vertical<R,C,Precision,Base,
            matrix_accumulate_vertical_functor, Vector<> >( m, min_functor_inst );
    }
    template<int R, int C, typename Precision, typename Base> inline Vector<>& max_vertical( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_vertical_functor<Precision,Internal::comp_greater<Precision> > matrix_accumulate_vertical_functor;
        matrix_accumulate_vertical_functor max_functor_inst( m.num_cols() );
        return Internal::accumulate_vertical<R,C,Precision,Base,
            matrix_accumulate_vertical_functor, Vector<> >( m, max_functor_inst );
    }
    template<int R, int C, typename Precision, typename Base> inline Vector<>& min_horizontal( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_horizontal_functor<Precision,Internal::comp_less<Precision> > matrix_accumulate_horizontal_functor;
        matrix_accumulate_horizontal_functor min_functor_inst( m.num_rows() );
        return Internal::accumulate_horizontal<R,C,Precision,Base,
            matrix_accumulate_horizontal_functor, Vector<> >( m, min_functor_inst );
    }
    template<int R, int C, typename Precision, typename Base> inline Vector<>& max_horizontal( const Matrix<R, C, Precision, Base> & m) {
        typedef Internal::accumulate_horizontal_functor<Precision,Internal::comp_greater<Precision> > matrix_accumulate_horizontal_functor;
        matrix_accumulate_horizontal_functor max_functor_inst( m.num_rows() );
        return Internal::accumulate_horizontal<R,C,Precision,Base,
            matrix_accumulate_horizontal_functor, Vector<> >( m, max_functor_inst );
    }
}
#endif
