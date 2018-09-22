#pragma once
#include<iostream>
#include"common.hpp"
/*** automatic conversions from non-eigen types (sequences) ***/

/* template to define custom converter from sequence/list or approriate length and type, to eigen's Vector
	- length is stored in VT::RowsAtCompileTime
	- type is VT::Scalar
*/

template<typename T>
bool pySeqItemCheck(PyObject* o, int i){
	try{ py::cast<T>(PySequence_GetItem(o,i)); return true; } catch(...){ return false; }
#if 0
	return py::isinstance<T>(PySequence_GetItem(o,i));
#endif
}

template<typename T>
T pySeqItemExtract(PyObject* o, int i){ return py::cast<T>(PySequence_GetItem(o,i)); }


template<class VT>
bool VectorT_load_from_sequence(py::handle src, VT& value){
	if(!PySequence_Check(src.ptr())){ std::cerr<<"VectorT_load_from_sequence: rejecting non-sequence."<<std::endl;  return false; }
	size_t len=PySequence_Size(src.ptr());
	for(size_t i=0; i<len; i++) if(!pySeqItemCheck<typename VT::Scalar>(src.ptr(),i)){
		std::cerr<<"VectorT_load_from_sequence: rejecting sequence of size "<<len<<std::endl;
		return false;
	}
	if(VT::RowsAtCompileTime!=Eigen::Dynamic){ len=VT::RowsAtCompileTime; }
	else{ len=PySequence_Size(src.ptr()); value.resize(len); }
	for(size_t i=0; i<len; i++) value[i]=pySeqItemExtract<typename VT::Scalar>(src.ptr(),i);
	return true;
};

template<class MT>
bool MatrixT_load_from_sequences(py::handle src, MT& mx){
	if(!PySequence_Check(src.ptr())) return false;
	int sz=PySequence_Size(src.ptr());
	if(sz<1) return false; // 0-length sequence: fails
	bool isFlat=!PySequence_Check(PySequence_GetItem(src.ptr(),0));
	// mixed static/dynamic not handled (also not needed)
	static_assert(
		(MT::RowsAtCompileTime!=Eigen::Dynamic && MT::ColsAtCompileTime!=Eigen::Dynamic)
		||
		(MT::RowsAtCompileTime==Eigen::Dynamic && MT::ColsAtCompileTime==Eigen::Dynamic)
	);
	if(MT::RowsAtCompileTime!=Eigen::Dynamic){
		if(isFlat){
			// flat sequence (first item not sub-sequence), must contain exactly all items
			if(sz!=MT::RowsAtCompileTime*MT::ColsAtCompileTime) return false;
		} else {
			// contains nested sequences, one per row
			if(sz!=MT::RowsAtCompileTime) return false;
		}
	} else {
		// find the right size
		if(isFlat) mx.resize(sz,1); // row vector, if flat
		else{ // find maximum size of items
			int rows=sz; int cols=0;
			for(int i=0; i<rows; i++){
				if(!PySequence_Check(PySequence_GetItem(src.ptr(),i))) throw std::runtime_error("Some elements of the array given are not sequences");
				int cols2=PySequence_Size(PySequence_GetItem(src.ptr(),i));
				if(cols==0) cols=cols2;
				if(cols!=cols2) throw std::runtime_error("Not all sub-sequences have the same length when assigning dynamic-sized matrix.");
			}
			mx.resize(rows,cols);
		}
	}
	if(isFlat){
		if(sz!=mx.rows()*mx.cols()) throw std::runtime_error("Assigning matrix "+std::to_string(mx.rows())+"x"+std::to_string(mx.cols())+" from flat vector of size "+std::to_string(sz));
		for(int i=0; i<sz; i++){
			mx(i/mx.rows(),i%mx.cols())=pySeqItemExtract<typename MT::Scalar>(src.ptr(),i);
		}
	} else {
		for(Index row=0; row<mx.rows(); row++){
			if(row>=PySequence_Size(src.ptr())) throw std::runtime_error("Sequence rows of size "+std::to_string(sz)+" too short for assigning matrix with "+std::to_string(mx.rows())+" rows.");
			py::handle rowSeq(PySequence_GetItem(src.ptr(),row));
			if(!PySequence_Check(rowSeq.ptr())) throw std::runtime_error("Element of row sequence not a sequence.");
			if(mx.cols()!=PySequence_Size(rowSeq.ptr())) throw std::runtime_error("Row "+std::to_string(row)+": should specify exactly "+std::to_string(mx.cols())+" numbers, has "+std::to_string(PySequence_Size(rowSeq.ptr())));
			for(Index col=0; col<mx.cols(); col++){
				mx(row,col)=pySeqItemExtract<typename MT::Scalar>(rowSeq.ptr(),col);
			}
		}
	}
	return true;
}

// create AlignedBoxNr from tuple of 2 Vector3r's
template<class AlignedBoxT>
bool AlignedBoxT_load_from_tuple(py::handle src, AlignedBoxT& box){
	if(!PySequence_Check(src.ptr())) return false;
	if(PySequence_Size(src.ptr())!=2) return false;
	typedef typename AlignedBoxT::VectorType VectorT;
	if(!pySeqItemCheck<VectorT>(src.ptr(),0) || !pySeqItemCheck<VectorT>(src.ptr(),1)) return false;
	box.min()=pySeqItemExtract<VectorT>(src.ptr(),0);
	box.max()=pySeqItemExtract<VectorT>(src.ptr(),1);
	return true;
}

template<class QuaternionT>
bool QuaternionT_load_from_axisAngle_angleAxis(py::handle src, QuaternionT& quat){
	typedef typename QuaternionT::Scalar Scalar;
	typedef Eigen::Matrix<Scalar,3,1> Vector3T;
	if(!PySequence_Check(src.ptr())) return false;
	if(PySequence_Size(src.ptr())!=2) return false;
	py::handle a(PySequence_GetItem(src.ptr(),0)), b(PySequence_GetItem(src.ptr(),1));
	// axis-angle or angle-axis
	if(py::isinstance<Vector3T>(a) && py::isinstance<Scalar>(b)){
		quat=QuaternionT(AngleAxisr(py::cast<Scalar>(b),py::cast<Vector3T>(a)));
		return true;
	}
	if(py::isinstance<Vector3T>(b) && py::isinstance<Scalar>(a)){
		quat=QuaternionT(AngleAxisr(py::cast<Scalar>(a),py::cast<Vector3T>(b)));
		return true;
	}
	return false;
}



