//##########################################################################
//#                                                                        #
//#                            CLOUDCOMPARE                                #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
//#                                                                        #
//##########################################################################

#ifndef CC_INDEXED_TRANSFORMATION_HEADER
#define CC_INDEXED_TRANSFORMATION_HEADER

//Local
#include "qCC_db.h"
#include "ccGLMatrix.h"

//! A 4x4 'transformation' matrix (column major order) associated to an index (typically a timestamp)
class QCC_DB_LIB_API ccIndexedTransformation : public ccGLMatrix
{
public:

	//! Default constructor
	/** Matrix is set to identity (see ccGLMatrix::toIdentity) by default.
		Index is set to zero by default.
	**/
	ccIndexedTransformation(); //默认构造函数

	//! Constructor from a transformation matrix
	/** Index is set to zero by default.
		\param matrix transformation matrix
	**/
	ccIndexedTransformation(const ccGLMatrix& matrix); //矩阵中构建

	//! Constructor from a transformation matrix and an index
	/** \param matrix transformation matrix
		\param index associated index (e.g. timestamp)
	**/
	ccIndexedTransformation(const ccGLMatrix& matrix, double index);

	//! Copy constructor
	ccIndexedTransformation(const ccIndexedTransformation& trans);

	//! Returns associated index (e.g. timestamp)
	inline double getIndex() const { return m_index; }

	//! Sets associated index (e.g. timestamp)
	inline void setIndex(double index) { m_index = index; }

	//给定两个变换，插值得到第三个
	//! Interpolates two transformations at an absolute position (index)
	/** Warning: interpolation index must lie between the two input matrices indexes!
		\param interpIndex interpolation position (should be between trans1 and trans2 indexes).
		\param trans1 first transformation
		\param trans2 second transformation
	**/
	static ccIndexedTransformation Interpolate(double interpIndex, const ccIndexedTransformation& trans1, const ccIndexedTransformation& trans2);

	//! Multiplication by a ccGLMatrix operator
	ccIndexedTransformation operator * (const ccGLMatrix& mat) const; //旋转

	//! (in place) Multiplication by a ccGLMatrix operator
	/** Warning: index is not modified by this operation.
	**/
	ccIndexedTransformation& operator *= (const ccGLMatrix& mat); //旋转

	//! Multiplication operator
	//ccIndexedTransformation operator * (const ccIndexedTransformation& mat) const;
	
	//! (in place) Multiplication operator
	/** Warning: index is not modified by this operation.
	**/
	//ccIndexedTransformation& operator *= (const ccIndexedTransformation& trans)

	//! (in place) Translation operator
	/** Warning: index is not modified by this operation.
	**/
	ccIndexedTransformation& operator += (const CCVector3& T); //平移操作
	//! (in place) Translation operator
	/** Warning: index is not modified by this operation.
	**/
	ccIndexedTransformation& operator -= (const CCVector3& T); //平移操作

	//! Returns transposed transformation
	/** Warning: index is not modified by this operation.
	**/
	ccIndexedTransformation transposed() const;  //转置， 索引不变

	//! Returns inverse transformation
	/** Warning: index is not modified by this operation.
	**/
	ccIndexedTransformation inverse() const;  //逆变换, 索引不变

	//inherited from ccGLMatrix  // 从 ccGLMatrix 中继承
	virtual bool toAsciiFile(QString filename, int precision = 12) const;
	virtual bool fromAsciiFile(QString filename);

	//inherited from ccSerializableObject  // 从 ccSerializableObject 中继承
	virtual bool isSerializable() const { return true; }
	virtual bool toFile(QFile& out) const;
	virtual bool fromFile(QFile& in, short dataVersion, int flags);

protected:

	//! Associated index (e.g. timestamp)
	double m_index;   //索引值
};

#endif //CC_INDEXED_TRANSFORMATION_HEADER
