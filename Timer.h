#ifndef TIMER_H
#define TIMER_H
#include <General.h>
#include <Thread.h>

#define TIME_ONESHOT    0x0000   /* program timer for single event */
#define TIME_PERIODIC   0x0001   /* program for continuous periodic event */

// Rappelle une callback � intervalle de temps r�gulier
// dans un thread � part
class CTimer : public CThread {
	typedef void* (*pFctCallback)(void*);

public:
	CTimer();
	virtual ~CTimer();

	inline const uint64_t ui64GetDuree() const { return ui64TpsUsec;};
	inline void SetDuree(uint64_t ui64_Duree) {ui64TpsUsec=ui64_Duree;};

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
