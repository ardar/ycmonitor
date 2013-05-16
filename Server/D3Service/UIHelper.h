#pragma once

class UIHelper
{
public:
	UIHelper(void);
	virtual ~UIHelper(void);

	void UpdateServer(ServiceStatusInfo& node);
	void UpdateSession(PcSessionInfo& session);

};
