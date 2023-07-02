#include "BillboardImage.h"

void BillboardImage::Init(TextureHandle texture, Vector2 size)
{
	mImage = Image3D(texture, size, false);
	mImage.SetParent(this);
}

void BillboardImage::Update(const ViewProjection& vp)
{
	Matrix4 mat = vp.mView;
	mat = -mat;
	if (mUseBillboardY) {
		Vector3 yVec = Vector3( 0, 1, 0 ).GetNormalize();
		Vector3 xVec = Vector3(mat[0][0], mat[0][1], mat[0][2]).GetNormalize();
		Vector3 zVec = xVec.Cross(yVec).GetNormalize();

		mat[1][0] = yVec.x;
		mat[1][1] = yVec.y;
		mat[1][2] = yVec.z;
		mat[2][0] = zVec.x;
		mat[2][1] = zVec.y;
		mat[2][2] = zVec.z;
	}
	
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

	mTransform.UpdateMatrix();
	mTransform.matrix = mat * mTransform.matrix;

	mImage.mTransform.UpdateMatrix();
	TransferBuffer(vp);
}

void BillboardImage::TransferBuffer(ViewProjection v) {
	mImage.TransferBuffer(v);
}

void BillboardImage::Draw() {
	mImage.Draw();
}

void BillboardImage::DrawCommands()
{
	mImage.DrawCommands();
}
