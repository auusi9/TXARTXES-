#include "MCC.h"
#include "UCC.h"
#include "AgentContainer.h"

enum State
{
	ST_INIT,
	ST_REGISTERING,
	ST_IDLE,

	// TODO 1: Add other states ...
	ST_NEGOTIATING,
	ST_NEGOTIATION_END,

	ST_UNREGISTERING,
	ST_FINISHED
};

MCC::MCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId) :
	Agent(node),
	_contributedItemId(contributedItemId),
	_constraintItemId(constraintItemId)
{
	setState(ST_INIT);
}


MCC::~MCC()
{
}

void MCC::update()
{
	switch (state())
	{
	case ST_INIT:
		if (registerIntoYellowPages()) {
			setState(ST_REGISTERING);
		} else {
			setState(ST_FINISHED);
		}
		break;

	// TODO 2: Handle other states

	case ST_REGISTERING:
		break;

	case ST_NEGOTIATING:
		if (_ucc->negotiationFinished() == true) {
			if (_ucc->negotiationAgreement() == true) {
				setState(ST_NEGOTIATION_END);
			}
			else {
				setState(ST_IDLE);
			}
			destroyChildUCC();
		}
		break;

	case ST_UNREGISTERING:
		break;
	
	case ST_FINISHED:
		finish();
	}
}

void MCC::finalize()
{
	// TODO 3
	destroyChildUCC();
	//finish();
	unregisterFromYellowPages();
	setState(ST_UNREGISTERING);
}


void MCC::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	const PacketType packetType = packetHeader.packetType;
	if (state() == ST_REGISTERING && packetType == PacketType::RegisterMCCAck) {
		setState(ST_IDLE);
		socket->Disconnect();
	}
	else if (state() == ST_UNREGISTERING && packetType == PacketType::UnregisterMCCAck) {
		setState(ST_FINISHED);
		socket->Disconnect();
	}	// TODO 4 handle other requests
	else if (state() == ST_IDLE && packetType == PacketType::NegotiationProposalRequest) {
		createChildUCC();

		// Create message header and data
		PacketHeader packetHead;
		packetHead.packetType = PacketType::NegotiationProposalAnswer;
		packetHead.srcAgentId = id();
		packetHead.dstAgentId = packetHeader.srcAgentId;
		PacketNegotiationProposalAnswer packetData;
		packetData.uccID = _ucc->id();

		// Serialize message
		OutputMemoryStream stream;
		packetHead.Write(stream);
		packetData.Write(stream);

		const std::string hostIP = socket->RemoteAddress().GetIPString();
		const int port = LISTEN_PORT_AGENTS;

		sendPacketToHost(hostIP, port, stream);

		setState(ST_NEGOTIATING);
	}
}

bool MCC::negotiationFinished() const
{
	// TODO 5
	bool answer = state() == ST_NEGOTIATION_END;
	return answer;
}

bool MCC::negotiationAgreement() const
{
	// TODO 6
	bool answer = state() == ST_NEGOTIATION_END;
	return answer;
}

bool MCC::registerIntoYellowPages()
{
	// Create message header and data
	PacketHeader packetHead;
	packetHead.packetType = PacketType::RegisterMCC;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = -1;
	PacketRegisterMCC packetData;
	packetData.itemId = _contributedItemId;

	// Serialize message
	OutputMemoryStream stream;
	packetHead.Write(stream);
	packetData.Write(stream);

	return sendPacketToYellowPages(stream);
}

void MCC::unregisterFromYellowPages()
{
	// Create message
	PacketHeader packetHead;
	packetHead.packetType = PacketType::UnregisterMCC;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = -1;
	PacketUnregisterMCC packetData;
	packetData.itemId = _contributedItemId;

	// Serialize message
	OutputMemoryStream stream;
	packetHead.Write(stream);
	packetData.Write(stream);

	sendPacketToYellowPages(stream);
}

void MCC::createChildUCC()
{
	// TODO 7
	_ucc.reset(new UCC(node(), _contributedItemId, _constraintItemId));
	g_AgentContainer->addAgent(_ucc);
}

void MCC::destroyChildUCC()
{
	// TODO 8
	if (_ucc != nullptr) {
		_ucc->finalize();
		_ucc.reset();
	}
}
