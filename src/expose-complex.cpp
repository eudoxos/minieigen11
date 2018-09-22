#include"visitors.hpp"
void expose_complex(py::module& mod){
	#ifdef _COMPLEX_SUPPORT
		py::class_<Vector2cr> cV2cr(mod,"Vector2c","/*TODO*/"); VectorVisitor<Vector2cr>::visit(cV2cr);
		py::class_<Vector3cr> cV3cr(mod,"Vector3c","/*TODO*/"); VectorVisitor<Vector3cr>::visit(cV3cr);
		py::class_<Vector6cr> cV6cr(mod,"Vector6c","/*TODO*/"); VectorVisitor<Vector6cr>::visit(cV6cr);
		py::class_<VectorXcr> cVXcr(mod,"VectorXc","/*TODO*/"); VectorVisitor<VectorXcr>::visit(cVXcr);

		py::class_<Matrix3cr> cM3cr(mod,"Matrix3c","/*TODO*/"); MatrixVisitor<Matrix3cr>::visit(cM3cr);
		py::class_<Matrix6cr> cM6cr(mod,"Matrix6c","/*TODO*/"); MatrixVisitor<Matrix6cr>::visit(cM6cr);
		py::class_<MatrixXcr> cMXcr(mod,"MatrixXc","/*TODO*/"); MatrixVisitor<MatrixXcr>::visit(cMXcr);
		;
	#endif
}
