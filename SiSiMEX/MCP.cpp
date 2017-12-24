#include "MCP.h"
#include "UCP.h"
#include "AgentContainer.h"


enum State
{
	ST_INIT,
	ST_REQUESTING_MCCs,
	ST_SELECTING_MCC,
	ST_NEGOTIATION_START,
	ST_NEGOTIATING,
	ST_NEGOTIATION_END
};

MCP::MCP(Node *node, uint16_t itemId) :
	Agent(node),
	_itemId(itemId),
	_negotiationAgreement(false)
{
	setState(ST_INIT);
}

MCP::~MCP()
{
}

void MCP::update()
{
	switch (state())
	{
	case ST_INIT:
		queryMCCsForItem(_itemId);
		setState(ST_REQUESTING_MCCs);
		break;
	case ST_SELECTING_MCC:
		selectMCC();
		break;
	case ST_NEGOTIATION_START:
		//Nothing to do here
		break;
	case ST_NEGOTIATING:
		if (_ucp->negotiationFinished())
		{
			if (_ucp->negotiationAgreement())
			{
				//Negotiation succeed
			}
			else
			{
				//Negotiation Failed
			}
		}
		break;
	case ST_NEGOTIATION_END:
		break;
	// TODO : Handle other states

	default:;
	}
}

void MCP::selectMCC()
{
	if (_mccRegisterIndex >= _mccRegisters.size())
	{
		sendNegotiationRequest(_mccRegisters[_mccRegisterIndex]);
		setState(ST_NEGOTIATION_START);
	}
	else
		setState(ST_NEGOTIATION_END);
}

void MCP::finalize()
{
	destroyChildUCP();
	finish();
}

void MCP::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	const PacketType packetType = packetHeader.packetType;
	if (state() == ST_REQUESTING_MCCs && packetType == PacketType::ReturnMCCsForItem)
	{
		iLog << "OnPacketReceived PacketType::ReturnMCCsForItem " << _itemId;

		// Read the packet
		PacketReturnMCCsForItem packetData;
		packetData.Read(stream);

		for (auto &mccdata : packetData.mccAddresses)
		{
			uint16_t agentId = mccdata.agentId;
			const std::string &hostIp = mccdata.hostIP;
			uint16_t hostPort = mccdata.hostPort;
			iLog << " - MCC: " << agentId << " - host: " << hostIp << ":" << hostPort;
		}

		// Collect MCC compatible from YP
		_mccRegisters.swap(packetData.mccAddresses);

		// Select MCC to negotiate
		_mccRegisterIndex = 0;
		setState(ST_SELECTING_MCC);

		socket->Disconnect();
	}
	else if (state() == ST_NEGOTIATION_START && packetType == PacketType::NegotiationProposalAnswer)
	{
		setState(ST_NEGOTIATING);

		PacketNegotiationProposalAnswer packetData;
		packetData.Read(stream);

		AgentLocation uccLoc;
		uccLoc.hostIP = socket->RemoteAddress().GetIPString();
		uccLoc.hostPort = LISTEN_PORT_AGENTS;
		uccLoc.agentId = packetData.uccID;

		createChildUCP(uccLoc);
	}

	// TODO 3: Handle other responses
}

bool MCP::negotiationFinished() const
{
	// TODO 
	return false;
}

bool MCP::negotiationAgreement() const
{
	// TODO 
	return false;
}


bool MCP::queryMCCsForItem(int itemId)
{
	// Create message header and data
	PacketHeader packetHead;
	packetHead.packetType = PacketType::QueryMCCsForItem;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = -1;
	PacketQueryMCCsForItem packetData;
	packetData.itemId = _itemId;

	// Serialize message
	OutputMemoryStream stream;
	packetHead.Write(stream);
	packetData.Write(stream);

	// 1) Ask YP for MCC hosting the item 'itemId'
	return sendPacketToYellowPages(stream);
}

bool MCP::sendNegotiationRequest(const AgentLocation &mccRegister)
{
	// TODO 
	PacketHeader packetHead;
	packetHead.packetType = PacketType::NegotiationProposalRequest;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = mccRegister.agentId;

	OutputMemoryStream stream;
	packetHead.Write(stream);

	return sendPacketToHost(mccRegister.hostIP, mccRegister.hostPort,stream);
}

void MCP::createChildUCP(const AgentLocation &uccLoc)
{
	// TODO 
	destroyChildUCP();
	_ucp.reset(new UCP(node(), _itemId, uccLoc));
	g_AgentContainer->addAgent(_ucp);
}

void MCP::destroyChildUCP()
{
	// TODO 
	if (_ucp != nullptr) {
		_ucp->finalize();
		_ucp.reset();
	}
}
