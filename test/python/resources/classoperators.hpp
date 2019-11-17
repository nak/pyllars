#ifndef __CLASSES__
#define __CLASSES__

namespace trial{

    namespace operators{

        class FullOperatorList{
        public:

            FullOperatorList(double d=0.134):value(d){}

            FullOperatorList& operator+(){
                return *this;
            }

            int operator-() const{
                return -int(value);
            }

            FullOperatorList operator~() const{
                return FullOperatorList(-value - 1.2);
            }

            FullOperatorList operator+(const int i) const{
                return FullOperatorList(value + i);
            }
            FullOperatorList operator-(const int i) const{
                return FullOperatorList(value - i);
            }
            FullOperatorList operator*(const double factor) const{
                return FullOperatorList(value * factor);
            }

            FullOperatorList operator/(const float div) const{
                return FullOperatorList(value/double(div));
            }

            FullOperatorList operator&(const int i) const{
                return FullOperatorList( int(value) & i );
            }

            FullOperatorList operator|(const int i) const{
                return FullOperatorList( int(value) | i );
            }

            FullOperatorList operator^(const int i) const{
                return FullOperatorList( int(value) ^ i );
            }

            FullOperatorList operator%(const int i) const{
                return FullOperatorList( int(value) % i);
            }

            FullOperatorList operator<<(const int i) const{
                return FullOperatorList( int(value) << i );
            }

            FullOperatorList operator>>(const int i){
                return FullOperatorList( int(value) >> i );
            }

            FullOperatorList& operator+=(const int i){
                value += i;
                return *this;
            }

            FullOperatorList& operator-(const int i){
                value -= i;
                return *this;
            }

        private:
            double value;
        };
    }
}

#endif
