#include <Thread.h>
#ifndef WIN32
#include <stdint.h>
#else
#include <windows.h>

#ifndef uint64_t
#define uint64_t ULONGLONG 
#endif
#endif //WIN32
// Rappelle une callback à intervalle de temps régulier
// dans un thread à part
class CTimer : public CThread {
	typedef void(*pFctCallback)(void*);

public:
	CTimer();
	virtual ~CTimer();

	inline const bool IsStarted() const { return m_bStarted; };
	inline const uint64_t ui64GetDuree() const { return ui64TpsUsec;};
	
	void Start(uint64_t ui64_usec=0, void *h_objet_attache=NULL);
	void Stop();

	void SetCallback(pFctCallback callback);
	inline pFctCallback GetCallBack() { return m_hCallBack;};	
	inline void *GetObjetAttache() const { return m_hObjetAttache;};
	
	void *Thread(void *pThis);
private: // Data
	int iIdPipe [2];
	bool m_bStarted;
	uint64_t ui64TpsUsec;
	pFctCallback m_hCallBack;
	void * m_hObjetAttache;
private: // Fonction
	void Init();
};

