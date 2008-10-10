#include <vector>
using namespace std;
enum ATOMIC_TYPE {ATOMIC_SCALAR, ATOMIC_SYMBOL, ATOMIC_SIGN};
enum SIGN_TYPE {SIGN_PLUS, SIGN_MINUS, SIGN_MULTIPLY};

class CAtomic{
public:

   CAtomic() {}
   CAtomic(ATOMIC_TYPE type, float scalar) : type(type), scalar(scalar){
   }
   CAtomic(ATOMIC_TYPE type, int symbolOrSign) : type(type), symbol(symbolOrSign) {
   }

   ATOMIC_TYPE type;
   union{
      float scalar;
      int symbol;
      int sign;
   };
};

class CExpression{
public:
   CExpression() {}
   CExpression(const CAtomic& atom) {
      expr.push_back(atom);
   }
   CExpression& operator = (const CExpression& ex) {
      expr = ex.expr;
      return *this;
   }
   CExpression(const CExpression& ex) {
      expr = ex.expr;
   }
   CExpression(int symbol, float coef) {
      expr.push_back(CAtomic(ATOMIC_SYMBOL, symbol));
      expr.push_back(CAtomic(ATOMIC_SCALAR, coef));
      expr.push_back(CAtomic(ATOMIC_SIGN, SIGN_MULTIPLY));

   }

   CExpression(CAtomic& atomic) {
      expr.push_back(atomic);
   }

   inline void Append(const CExpression& it, CAtomic& sign) {
      int mySize = expr.size();
      int itSize = it.expr.size();
      expr.resize(mySize + itSize + 1);
      memcpy(&expr[mySize], &it.expr[0], sizeof(CAtomic) * itSize);
      expr[mySize + itSize] = sign;
   }

   CExpression& operator += (const CExpression& it) {
      Append(it, CAtomic(ATOMIC_SIGN, SIGN_PLUS));
      return *this;
   }

   CExpression& operator -= (const CExpression& it) {
      Append(it, CAtomic(ATOMIC_SIGN, SIGN_MINUS));
      return *this;
   }

   CExpression& operator *= (const CExpression& it) {
      Append(it, CAtomic(ATOMIC_SIGN, SIGN_MULTIPLY));
      return *this;
   }

   CExpression& operator += (const CAtomic& at) {
      expr.push_back(at);
      expr.push_back(CAtomic(ATOMIC_SIGN, SIGN_PLUS));
      return *this;
   }

   CExpression& operator -= (const CAtomic& at) {
      expr.push_back(at);
      expr.push_back(CAtomic(ATOMIC_SIGN, SIGN_MINUS));
      return *this;
   }

   CExpression& operator *= (const CAtomic& at) {
      expr.push_back(at);
      expr.push_back(CAtomic(ATOMIC_SIGN, SIGN_MULTIPLY));
      return *this;
   }

   vector<CAtomic> expr;
};

