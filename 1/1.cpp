#include <condition_variable>
#include <cstddef>
#include <deque>
#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <vector>
#include <unistd.h>
#include <omp.h>
#include <mutex>
#include <stdexcept>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::istream;
using std::ostream;
using std::string;
using std::swap;
using std::vector;

class BigInteger {
 private:
  vector<int> numbers;
  bool isPositive;

 public:

  BigInteger();
  BigInteger(const vector<int>& numbers, bool isPositive);
  BigInteger(int int1);
  BigInteger(const BigInteger &BigInt);
  BigInteger(const string& str);

  ~BigInteger() { };

  BigInteger operator-() const;

  friend bool operator == (const BigInteger &left, const BigInteger &right);
  friend bool operator != (const BigInteger &left, const BigInteger &right);
  friend bool operator < (const BigInteger &left, const BigInteger &right);
  friend bool operator > (const BigInteger &left, const BigInteger &right);
  friend bool operator <= (const BigInteger &left, const BigInteger &right);
  friend bool operator >= (const BigInteger &left, const BigInteger &right);

  BigInteger& operator += (const BigInteger& num);
  BigInteger& operator -= (const BigInteger& num);
  BigInteger& operator *= (const BigInteger& num);
  BigInteger& operator /= (const BigInteger& num);
  BigInteger& operator %= (const BigInteger& num);

  BigInteger& operator++();
  BigInteger& operator--();
  BigInteger operator++(int);
  BigInteger operator--(int);

  friend BigInteger operator + (const BigInteger& left, const BigInteger& right);
  friend BigInteger operator - (const BigInteger& left, const BigInteger& right);
  friend BigInteger operator * (const BigInteger& left, const BigInteger& right);
  friend BigInteger operator / (const BigInteger& left, const BigInteger& right);
  friend BigInteger operator % (const BigInteger& left, const BigInteger& right);

  friend BigInteger operator + (int first, const BigInteger &second);
  friend BigInteger operator - (int first, const BigInteger &second);
  friend BigInteger operator * (int first, const BigInteger &second);
  friend BigInteger operator / (int first, const BigInteger &second);
  friend BigInteger operator % (int first, const BigInteger &second);

  operator bool() const { return (numbers.size() > 1 || numbers[0] != 0); }

  string toString() const;

  friend ostream& operator<< (ostream &os, const BigInteger &big_int);
  friend istream& operator>> (istream &is, BigInteger &big_int);

};

BigInteger::BigInteger(){
  isPositive = true;
  numbers = {0};
}

BigInteger::BigInteger(int int1) {
  isPositive = ( int1 >= 0 );
  if ( int1 == 0 ) {
    numbers.push_back(0);
  } else {
    if(int1 < 0 ) {
      int1 = -int1;
    }
    while( int1 > 0 ) {
      numbers.push_back( int1 % 10 );
      int1 = int1 / 10;
    }
  }
}

BigInteger::BigInteger(const BigInteger &BigInt) {
  isPositive = BigInt.isPositive;
  numbers = BigInt.numbers;
}

BigInteger::BigInteger(const vector<int>& numbers_, bool isPositive_) {
  isPositive = isPositive_;
  if(numbers_.empty()){
    numbers = { 0 };
  } else {
    numbers = numbers_;
  }
}

BigInteger::BigInteger(const string &str) {
  isPositive = (str[0] != '-');
  for (unsigned long i = str.size() - 1; i >= 1; i--) {
    numbers.push_back(str[i]-'0');
  }
  if( isPositive ){
    numbers.push_back(str[0] - '0');
  }
}

BigInteger BigInteger::operator-() const {
  if ( numbers.size() == 1 && numbers[0] == 0 ){
    return BigInteger(0);
  } else {
    return BigInteger(numbers, !isPositive);
  }
}

bool operator == (const BigInteger& left, const BigInteger& right) {
  if (left.numbers.size() != right.numbers.size()) {
    return false;
  }
  if (right.numbers.size() == 1 && right.numbers[0] == 0 && left.numbers[0] == 0) {
    return true;
  }
  if (right.isPositive != left.isPositive) {
    return false;
  }
  for (unsigned long i = 0; i < right.numbers.size(); i++) {
    if (right.numbers[i] != left.numbers[i]) {
      return false;
    }
  }
  return true;
}

bool operator != (const BigInteger& left, const BigInteger& right) {
  return !(left == right);
}

bool operator < (const BigInteger& left, const BigInteger& right) {
  if (left.numbers.size() == 1 && left.numbers[0] == 0) {
    if (right.numbers.size() == 1 && right.numbers[0] == 0) {
      return false;
    } else {
      return right.isPositive;
    }
  }
  if( left.isPositive == right.isPositive ) {
    if (left.numbers.size() < right.numbers.size()) {
      return left.isPositive;
    }
    if (right.numbers.size() < left.numbers.size()) {
      return !left.isPositive;
    }
    for (unsigned long i = left.numbers.size() - 1; ; i--) {
      if (left.numbers[i] < right.numbers[i]) {
        return left.isPositive;
      }
      else if (left.numbers[i] > right.numbers[i]) {
        return !left.isPositive;
      }
      if( i == static_cast<unsigned long>(0) ) {
        break;
      }
    }
    return !left.isPositive;
  } else {
    return right.isPositive;
  }
}

bool operator <= (const BigInteger& left, const BigInteger& right) {
  return (left == right || left < right);
}

bool operator > (const BigInteger& left, const BigInteger& right) {
  return right < left;
}

bool operator >= (const BigInteger& left, const BigInteger& right) {
  return right <= left;
}

BigInteger operator + (const BigInteger &left, const BigInteger &right) {
  if( left.isPositive != right.isPositive){
    if(left.isPositive ){
      return left - BigInteger( right.numbers, !right.isPositive);
    }  else {
      BigInteger newLeft = BigInteger( left.numbers, !left.isPositive);
      return right - newLeft;
    }
  } else {
    vector<int> newNumbers;
    int leftNumeral, rightNumeral, decades = 0;
    unsigned long max_size = left.numbers.size() > right.numbers.size() ? left.numbers.size() : right.numbers.size();
    for ( unsigned long i = 0; i < max_size; i++) {
      leftNumeral = (i < left.numbers.size() ? left.numbers[i] : 0);
      rightNumeral = (i < right.numbers.size() ? right.numbers[i] : 0);
      newNumbers.push_back((leftNumeral + rightNumeral + decades) % 10);
      decades = (leftNumeral + rightNumeral + decades) / 10;
    }
    if (decades != 0) {
      newNumbers.push_back(decades);
    }
    return BigInteger(newNumbers, left.isPositive);
  }
}

BigInteger operator - (const BigInteger &left, const BigInteger &right) {
  if( left.isPositive != right.isPositive){
    return left + BigInteger(right.numbers, !right.isPositive);
  } else {
    if (left.isPositive) {
      if (left >= right) {
        vector<int> newNumbers;
        int leftNumeral, rightNumeral, decades = 0;
        for (unsigned long i = 0; i < left.numbers.size(); i++) {
          leftNumeral = left.numbers[i];
          rightNumeral = (i < right.numbers.size() ? right.numbers[i] : 0);
          newNumbers.push_back((leftNumeral - rightNumeral - decades + 10) % 10);
          decades = ((leftNumeral - rightNumeral - decades) < 0 ? 1 : 0);
        }
        for (unsigned long i = newNumbers.size() - 1; ; i--) {
          if (newNumbers[i] != 0) {
            break;
          }
          newNumbers.pop_back();
          if( i == static_cast<unsigned long>(0) ) {
            break;
          }
        }
        return BigInteger(newNumbers, true);
      } else {
        return -(right - left);
      }
    } else {
      BigInteger positiveLeft(left.numbers, true);
      BigInteger positiveRight(right.numbers, true);
      return positiveRight - positiveLeft;
    }
  }
}

BigInteger operator * (const BigInteger &left, const BigInteger &right) {
  if ((left.numbers.size() == 1 && left.numbers[0] == 0) ||
      (right.numbers.size() == 1 && right.numbers[0] == 0)) {
    return BigInteger(0);
  }
  int rightNumeral;
  BigInteger currentResult(0);
  for (unsigned long i = 0; i < right.numbers.size(); i++) {
    vector<int> newNumbers;
    int decades = 0;
    rightNumeral = right.numbers[i];
    for (size_t j = 0; j < i; j++) {
      newNumbers.push_back(0);
    }
    int leftNumeral = 0;
    for (int j : left.numbers) {
      leftNumeral = j;
      newNumbers.push_back((rightNumeral * leftNumeral + decades) % 10);
      decades = (rightNumeral * leftNumeral + decades) / 10;
    }
    if (decades != 0) {
      newNumbers.push_back(decades);
    }
    currentResult = currentResult + BigInteger(newNumbers, true);
  }
  currentResult.isPositive = left.isPositive == right.isPositive;
  return currentResult;
}

BigInteger operator / (const BigInteger &left, const BigInteger &right) {
  BigInteger positiveLeft(left.numbers, true);
  BigInteger positiveRight(right.numbers, true);
  if (positiveLeft < positiveRight) {
    return BigInteger(0);
  }
  vector<int> results;
  BigInteger previousBalance(0);
  for (long i = positiveLeft.numbers.size() - 1; i >= 0; --i) {
    BigInteger currentDividend = positiveLeft.numbers[i] + 10*previousBalance;
    int leftNumber = 0, rightNumber = 9;
    while (rightNumber - leftNumber > 1) {
      int mid = (leftNumber + rightNumber) >> 1;
      mid*positiveRight <= currentDividend ? leftNumber = mid : rightNumber = mid;
    }
    int ans = rightNumber*positiveRight <= currentDividend ? rightNumber : leftNumber;
    results.push_back(ans);
    previousBalance = currentDividend - ans * positiveRight;
  }
  unsigned long firstNotNullNumber = 0;
  for ( firstNotNullNumber = 0; firstNotNullNumber < results.size(); firstNotNullNumber++) {
    if ( results[firstNotNullNumber] != 0 ){
      break;
    }
  }
  if ( firstNotNullNumber == results.size() ) {
    return BigInteger(0);
  } else {
    for ( unsigned long i = 0; i < results.size() / 2; i++ ) {
      swap(results[i], results[results.size() - 1 - i]);
    }
    for (unsigned long i = results.size() - 1; ; i--) {
      if (results[i] != 0) {
        break;
      }
      results.pop_back();
      if( i == static_cast<unsigned long>(0) ) {
        break;
      }
    }
    BigInteger result(results, (left.isPositive == right.isPositive));
    return result;
  }
}

BigInteger operator % (const BigInteger &left, const BigInteger &right) {
  BigInteger div = left / right;
  return left - div * right;
}

BigInteger& BigInteger::operator +=(const BigInteger &other) {
  if( (*this).isPositive != other.isPositive){
    if((*this).isPositive ){
      if ((*this) >= other) {
        int leftNumeral, rightNumeral, decades = 0;
        for (unsigned long i = 0; i < (*this).numbers.size(); i++) {
          leftNumeral = (*this).numbers[i];
          rightNumeral = (i < other.numbers.size() ? other.numbers[i] : 0);
          if( i < (*this).numbers.size()){
            (*this).numbers[i] = (leftNumeral - rightNumeral - decades + 10) % 10;
          } else {
            (*this).numbers.push_back((leftNumeral - rightNumeral - decades + 10) % 10);
          }

          decades = ((leftNumeral - rightNumeral - decades) < 0 ? 1 : 0);
        }

        for (unsigned long i = (*this).numbers.size() - 1; ; i--) {
          if ((*this).numbers[i] != 0) {
            break;
          }
          (*this).numbers.pop_back();
          if( i == static_cast<unsigned long>(0) ) {
            break;
          }
        }
        (*this).isPositive=true;
        return *this;
      } else {
        int leftNumeral, rightNumeral, decades = 0;
        for (unsigned long i = 0; i < other.numbers.size(); i++) {
          leftNumeral = other.numbers[i];
          rightNumeral = (i < (*this).numbers.size() ? (*this).numbers[i] : 0);
          if( i < (*this).numbers.size()){
            (*this).numbers[i] = (leftNumeral - rightNumeral - decades + 10) % 10;
          } else {
            (*this).numbers.push_back((leftNumeral - rightNumeral - decades + 10) % 10);
          }
          decades = ((leftNumeral - rightNumeral - decades) < 0 ? 1 : 0);
        }
        for (unsigned long i = (*this).numbers.size() - 1; ; i--) {
          if ((*this).numbers[i] != 0) {
            break;
          }
          (*this).numbers.pop_back();
          if( i == static_cast<unsigned long>(0) ) {
            break;
          }
        }
        (*this).isPositive=false;
        return *this;

      }
    }  else {
      if ((*this) >= other) {
        int leftNumeral, rightNumeral, decades = 0;
        for (unsigned long i = 0; i < (*this).numbers.size(); i++) {
          leftNumeral = (*this).numbers[i];
          rightNumeral = (i < other.numbers.size() ? other.numbers[i] : 0);
          if( i < (*this).numbers.size()){
            (*this).numbers[i] = (leftNumeral - rightNumeral - decades + 10) % 10;
          } else {
            (*this).numbers.push_back((leftNumeral - rightNumeral - decades + 10) % 10);
          }

          decades = ((leftNumeral - rightNumeral - decades) < 0 ? 1 : 0);
        }

        for (unsigned long i = (*this).numbers.size() - 1; ; i--) {
          if ((*this).numbers[i] != 0) {
            break;
          }
          (*this).numbers.pop_back();
          if( i == static_cast<unsigned long>(0) ) {
            break;
          }
        }
        (*this).isPositive=false;
        return *this;
      } else {
        int leftNumeral, rightNumeral, decades = 0;
        for (unsigned long i = 0; i < other.numbers.size(); i++) {
          leftNumeral = other.numbers[i];
          rightNumeral = (i < (*this).numbers.size() ? (*this).numbers[i] : 0);
          if( i < (*this).numbers.size()){
            (*this).numbers[i] = (leftNumeral - rightNumeral - decades + 10) % 10;
          } else {
            (*this).numbers.push_back((leftNumeral - rightNumeral - decades + 10) % 10);
          }
          decades = ((leftNumeral - rightNumeral - decades) < 0 ? 1 : 0);
        }
        for (unsigned long i = (*this).numbers.size() - 1; ; i--) {
          if ((*this).numbers[i] != 0) {
            break;
          }
          (*this).numbers.pop_back();
          if( i == static_cast<unsigned long>(0) ) {
            break;
          }
        }
        (*this).isPositive=true;
        return *this;

      }
   }
  } else {
    int leftNumeral, rightNumeral, decades = 0;
    unsigned long max_size = (*this).numbers.size() > other.numbers.size() ? (*this).numbers.size() : other.numbers.size();
    for ( unsigned long i = 0; i < max_size; i++) {
      leftNumeral = (i < (*this).numbers.size() ? (*this).numbers[i] : 0);
      rightNumeral = (i < other.numbers.size() ? other.numbers[i] : 0);
      if( i < (*this).numbers.size()){
        (*this).numbers[i] = (leftNumeral + rightNumeral + decades) % 10;
      } else {
        (*this).numbers.push_back((leftNumeral + rightNumeral + decades) % 10);
      }
      decades = (leftNumeral + rightNumeral + decades) / 10;
    }
    if (decades != 0) {
      (*this).numbers.push_back(decades);
    }
    return *this;
  }
  
}

BigInteger& BigInteger::operator -= (const BigInteger &other) {
  *this = *this - other;
  return *this;
}

BigInteger& BigInteger::operator *= (const BigInteger &other) {
  *this = *this * other;
  return *this;
}

BigInteger& BigInteger::operator /= (const BigInteger &other) {
  *this = *this / other;
  return *this;
}

BigInteger& BigInteger::operator %= (const BigInteger &other) {
  *this = *this % other;
  return *this;
}

BigInteger& BigInteger::operator ++ () {
  *this = 1 + *this;
  return *this;
}

BigInteger BigInteger::operator ++ (int) {
  BigInteger BigInt = *this;
  ++*this;
  return BigInt;
}

BigInteger& BigInteger::operator -- () {
  *this = *this - BigInteger(1);
  return *this;
}

BigInteger BigInteger::operator -- (int) {
  BigInteger BigInt = *this;
  --*this;
  return BigInt;
}

BigInteger operator - (int left, const BigInteger &right) {
  return BigInteger(left) - right;
}

BigInteger operator + (int left, const BigInteger &right) {
  BigInteger BigInt(left);
  return BigInt + right;
}

BigInteger operator * (int left, const BigInteger &right) {
  if (left == 0) {
    BigInteger result(0);
    return result;
  } else {
    int positiveLeft = (left < 0 ? -left : left);
    vector<int> newNumbers;
    int decades = 0;
    for (int rightNumeral : right.numbers) {
      newNumbers.push_back((positiveLeft * rightNumeral + decades) % 10);
      decades = (positiveLeft * rightNumeral + decades) / 10;
    }
    if (decades != 0) {
      newNumbers.push_back(decades);
    }
    return ( (left > 0) == ( right.isPositive ) ? BigInteger(newNumbers, true) : BigInteger(newNumbers, false) );
  }
}

BigInteger operator / (int left, const BigInteger &right) {
  return BigInteger(left) / right;
}

BigInteger operator % (int left, const BigInteger &right) {
  return BigInteger(left) % right;
}

istream& operator >> (istream &input, BigInteger &BigInt) {
  string string1;
  input >> string1;
  BigInt = BigInteger(string1);
  return input;
}

ostream& operator << (ostream &output, const BigInteger &BigInt) {
  output << BigInt.toString();
  return output;
}

string BigInteger::toString() const {
  string s;
  if (numbers.size() == 1 && numbers[0] == 0) {
    s += "0";
    return s;
  }
  if ( !isPositive ) {
    s += '-';
  }
  for (unsigned long i = numbers.size() - 1; i < numbers.size(); i--) {
    s += ('0' + numbers[i]);
  }
  return s;
}


const int pthread_num_max = 20;
int pthread_num = 4;

bool c[pthread_num_max];
int k = 0;

const int n = 1000;

vector<BigInteger> q;

pthread_t tids[pthread_num_max];

pthread_mutex_t mutex;
pthread_mutex_t *m = &mutex;

BigInteger sum(0);

void* f1( void* )
{
  pthread_mutex_lock(m);
  int current_k = k;
  k++;
  pthread_mutex_unlock(m);

  BigInteger sum1(0);
  BigInteger a;

  for(int i = current_k; i < q.size(); i+=pthread_num) {
    sum1 += q[i];
  }

  pthread_mutex_lock(m);
  sum +=sum1;
  pthread_mutex_unlock(m);

  c[current_k] = true;
}

bool IsEnding(){
  for(int i =0;  i < pthread_num; i++) {
    if (!c[i]) {
      return false;
    }
  }
  return true;
}

int main(int argc, char* argv[]) {

  pthread_num=argc==1?1:strtol(argv[1], NULL, 10);



  FILE *file;

  if (!(file = fopen("input.txt", "r")))
  {
    printf("\nError Opening File\n");
    exit(1);
  }


  string s;
  char ch;
  int fl=0;
  while (!feof(file)){
    ch = fgetc(file);
    if(ch>='0' && ch<='9'){
      s+=ch;
      fl=1;
    }else{
      if(fl==1){
        q.push_back(s);
        s="";
        fl=0;
      }
    }
  }
  fclose(file);

  double begin, end, total;
  begin = omp_get_wtime();

  for(int i = 0;  i < pthread_num; i++) {
    c[i] = false;
  }

  for (int i = 0; i < pthread_num; i++)
  {
    pthread_create(&(tids[i]), NULL, &f1, NULL);
  }

  int res=0;

  for (int i = 0; i < pthread_num; i++)
  {
    pthread_join(tids[i], reinterpret_cast<void **>(res));
  }

  while(!IsEnding()) {};

  end = omp_get_wtime();
  total = end - begin;

  std::cout << pthread_num << " " << sum << " " << total << endl;
  return 0;
}
