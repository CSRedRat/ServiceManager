#ifndef __STATE__H__
#define __STATE__H__

#include <cstdio>
#include <memory>
#include <vector>
#include "s16.h"
#include "s16rr.h"
#include "sd-defs.h"

class SvcState
{
  protected:
    svc_t * m_svc;
    class SvcManager & m_manager;

  public:
    SvcState (svc_t * svc, SvcManager & manager)
        : m_svc (svc), m_manager (manager)
    {
    }
    virtual ~SvcState () {}
    virtual int loop_iteration () = 0;
    virtual int process_event (pt_info_t *) = 0;
};

class StopTermState : public SvcState
{
    unsigned int m_timer;

  public:
    StopTermState (svc_t * svc, SvcManager & manager);
    ~StopTermState () {}
    int loop_iteration () {}
    int process_event (pt_info_t *);
    bool timer_cb (unsigned int t);
};

class StartPreState : public SvcState
{
    unsigned int m_timer;
    bool m_failed;

  public:
    StartPreState (svc_t * svc, SvcManager & manager);
    ~StartPreState () {}
    int loop_iteration ();
    int process_event (pt_info_t *);
    bool timer_cb (unsigned int t);
};

/* This launches ExecStart for a simple service. */
class RunState : public SvcState
{
    unsigned int m_timer;
    bool m_failed;

  public:
    RunState (svc_t * svc, SvcManager & manager);
    ~RunState () {}
    int loop_iteration ();
    int process_event (pt_info_t *);
    bool timer_cb (unsigned int t) {}
};

/* This is the failure state. */
class MaintenanceState : public SvcState
{
  public:
    MaintenanceState (svc_t * svc, SvcManager & manager);
    ~MaintenanceState () {}
    int process_event (pt_info_t *);
    int loop_iteration () {}
    bool timer_cb (unsigned int t) {}
};

/* This launches ExecStartPost. */
class StartPostState : public SvcState
{
    unsigned int m_timer;
    bool m_failed;
    pid_t m_main_pid;
    std::vector<pid_t> m_pids;

  public:
    StartPostState (svc_t * svc, SvcManager & manager);
    ~StartPostState () {}
    int loop_iteration ();
    int process_event (pt_info_t *);
    bool timer_cb (unsigned int t) {}
};

class SvcStateFactory
{
    svc_t * m_svc;
    SvcManager & m_manager;

  public:
    SvcStateFactory (svc_t * svc, SvcManager & manager)
        : m_svc (svc), m_manager (manager)
    {
    }
    template <class T> std::shared_ptr<T> new_state ()
    {
        return std::shared_ptr<T> (new T (m_svc, m_manager));
    }
    std::shared_ptr<StartPreState> new_start_pre ()
    {
        return std::shared_ptr<StartPreState> (
            new StartPreState (m_svc, m_manager));
    }
};

#endif