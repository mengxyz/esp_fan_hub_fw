#if !defined(BASEMODULE_H)
#define BASEMODULE_H

class BaseModule
{
protected:
   bool isReady = false;

public:
   bool ready()
   {
      return this->isReady;
   }
};

#endif // BASEMODULE_H
