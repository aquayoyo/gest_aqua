#include <Thread.h>

// Rappelle une callback à intervalle de temps régulier
// dans un thread à part
class CTimer : public CThread {
	typedef void(*pFctCallback)(void*);

public:
	CTimer();
	virtual ~CTimer();

	inline const bool IsStarted() const { return m_bStarted; };
	inline const unsigned long GetDuree() const { return m_ulDuree_ms;};
	
	void Start(unsigned long usec=0, void *h_objet_attache=NULL);
	void Stop();

	void SetCallback(pFctCallback callback);
	inline pFctCallback GetCallBack() { return m_hCallBack;};	
	inline void *GetObjetAttache() const { return m_hObjetAttache;};
	
	void *Thread(void *pThis);
private: // Data
	bool m_bStarted;
	unsigned long m_ulDuree_ms;
	pFctCallback m_hCallBack;
	void * m_hObjetAttache;
private: // Fonction
	void Init();
};

