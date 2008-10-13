
#include <Math.h>
//
// By using Physics_t everywhere, we can easily change between single-precision and
// double-precision floating point numbers
//
typedef float Physics_t;

//
// Template for a Linked List of items.  The items are enumerated using the Next() method.
//
template<class T> struct LinkedList
{
	struct innerList
	{
		T *pItem;
		innerList *pNext;
	};
	innerList *pList, *pLast;

	LinkedList()
	{
		pList = pLast = NULL;
	}

	~LinkedList()
	{
		innerList *pLast;
		while( pList )
		{
			pLast = pList;
			pList = pList->pNext;
			delete pLast;
		}
	}

	void AddItem( T *pItem )
	{
		if( pList )
		{
			pLast->pNext = new innerList;
			pLast->pNext->pItem = pItem;
			pLast->pNext->pNext = NULL;
			pLast = pLast->pNext;
		}
		else
		{
			pList = pLast = new innerList;
			pList->pItem = pItem;
			pList->pNext = NULL;
		}
	}

	bool DeleteItem( T *pItem )
	{
		innerList *pCur = pList, *pPrev = NULL;

		while( pCur )
		{
			if( pCur->pItem == pItem )
			{
				if( pCur == pList )
				{
					if( pCur == pLast )
					{
						delete pList;
						pList = pLast = NULL;
					}
					else
					{
						pList = pList->pNext;
						delete pCur;
					}
				}
				else if( pCur == pLast )
				{
					pLast = pPrev;
					pPrev->pNext = NULL;
					delete pCur;
				}
				else
				{
					pPrev->pNext = pCur->pNext;
					delete pCur;
				}
				return true;
			}
			pPrev = pCur;
			pCur = pCur->pNext;
		}
		return false;
	}

	T *NextItem( void  **pCur )
	{
		if( *pCur )
		{
			*pCur = ((innerList *)*pCur)->pNext;
			if( *pCur )
				return ((innerList *)*pCur)->pItem;
			else
				return NULL;
		}
		else
		{
			*pCur = (void *)pList;
			if( pList )
				return pList->pItem;
			else
				return NULL;
		}
	}
};


#include "Physics_Vector3.h"
#include "Physics_LargeVector.h"
#include "Physics_Matrix3x3.h"
#include "Physics_SparseSymmetricMatrix.h"
#include "Physics_SymmetricMatrix.h"

#include "Physics_Force.h"
#include "Physics_Constraint.h"
#include "Physics_SpringForce.h"
#include "Physics_GravityForce.h"
//#include "Physics_DragForce.h"
//#include "Physics_WindForce.h"

#include "Physics_ParticleSystem.h"