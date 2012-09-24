#ifndef TIMER_H
#define TIMER_H
#include <General.h>
#include <Thread.h>
#ifndef WIN32
#include <stdint.h>
#else
#include <windows.h>

#ifndef uint64_t
#define uint64_t ULONGLONG 
#endif
#endif //WIN32

#define TIME_ONESHOT    0x0000   /* program timer for single event */
#define TIME_PERIODIC   0x0001   /* program for continuous periodic event */

// Rappelle une callback à intervalle de temps régulier
// dans un thread à part
class CTimer : public CThread {
	typedef void* (*pFctCallback)(void*);

public:
	CTimer();
	virtual ~CTimer();

	inline const uint64_t ui64GetDuree() const { return ui64TpsUsec;};
	
	void Start(uint64_t ui64_usec=0, void *h_objet_attache=NULL,unsigned int uiFlag=TIME_ONESHOT);
	void Stop();

	void SetCallback(pFctCallback callback);
	inline pFctCallback GetCallBack() { return m_hCallBack;};	
	inline void *GetObjetAttache() const { return m_hObjetAttache;};
	
	void *Thread(void *pThis);
private: // Data
	uint64_t ui64TpsUsec;
	pFctCallback m_hCallBack;
	void * m_hObjetAttache;

	unsigned int uiFlagEvent;
private: // Fonction
	void Init();
};

#endif //TIMER_H