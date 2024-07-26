

#include "../common/Singleton.h"
class GateServer : public Singleton<GateServer> {
 public:
  GateServer(/* args */);
  ~GateServer();
 public:
  int OpenServer();
  void OnStopServer();
  void MainThread();
  std::thread *_handle;
  bool IsRun;
};
