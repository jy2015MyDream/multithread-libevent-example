
#ifndef __SINGLETON_H_
#define __SINGLETON_H_
template <class T>
class Singleton {
 private:
 protected:
  Singleton(/* args */){}
  static T *_ins;

 public:
  ~Singleton(){}
  static T *GetIns() {
    if (_ins == nullptr) {
      _ins = new T();
    }
    return _ins;
  }
};
template <class T>
T *Singleton<T>::_ins = nullptr;
#endif