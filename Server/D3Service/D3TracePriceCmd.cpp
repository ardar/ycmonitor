#include "StdAfx.h"
#include "D3TracePriceCmd.h"

D3TracePriceCmd::D3TracePriceCmd(void)
{
	m_llMaxPrice = 0;
	m_llAddPrice = 0;
	m_dwSessionId = 0;
}

D3TracePriceCmd::~D3TracePriceCmd(void)
{
}

void D3TracePriceCmd::ReadPack( DWORD dwSessionId, BYTE* buf, int len )
{

}

void D3TracePriceCmd::Execute()
{
	CArray<D3BiddingItem*> result;
	/*D3ErrorCode error = GameProxy::Instance().GetBiddingItems(m_dwSessionId, result);
	if (error==ERR_Success)
	{
		D3Service::Instance().Log("D3TracePriceCmd:: ��ȡ�����б�ɹ�");
		for (int i=0;i<result.GetCount();i++)
		{
			D3BiddingItem* pItem = result.GetAt(i);
			if (m_szItemName.Compare(pItem->info.szName)!=0)
			{
				continue;
			}
			D3Service::Instance().Log("D3TracePriceCmd:: ������Ʒ %s :��ǰ����:%I64d", pItem->info.szName, pItem->llCurBid);
			if(pItem->status==D3BIDLOST && pItem->llCurBid < m_llMaxPrice)
			{
				LONGLONG llMyPrice = (pItem->llCurBid+m_llAddPrice);
				if (llMyPrice>m_llMaxPrice)
				{
					llMyPrice = m_llMaxPrice;
				}
				D3Service::Instance().Log("D3TracePriceCmd:: �۸񱻳���, ��ʼ���� :%I64d", llMyPrice);

				D3ErrorCode receive_error = GameProxy::Instance().Bid(
					m_dwSessionId, pItem->info.pLowData, pItem->info.nLowDataLen, llMyPrice);
				if (receive_error==ERR_Success)
				{
					D3Service::Instance().Log("D3TracePriceCmd:: ������Ʒ���۳ɹ� %s, ��ǰ���� %I64d", 
						pItem->info.szName, llMyPrice);
				}
				else
				{
					D3Service::Instance().Log("D3TracePriceCmd:: ������Ʒ����ʧ�� %s, ����:%d", 
						pItem->info.szName, receive_error);
				}
			}
		}
	}
	else
	{
		D3Service::Instance().Log("D3TracePriceCmd:: ��ȡ�����б�ʧ�� ����:%d", error);
	}*/
}

void D3TracePriceCmd::Cancel( DWORD dwErrorCode )
{
	D3Service::Instance().Log("D3TracePriceCmd:: Canceled :%d", dwErrorCode);
}
