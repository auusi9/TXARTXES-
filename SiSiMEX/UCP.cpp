#include "UCP.h"
#include "MCP.h"
#include "AgentContainer.h"


enum State
{
	ST_INIT,
	// TODO 1: Add other states...
	ST_OFFER_REQUEST,
	ST_SEARCHING_CONSTRAINT,
	ST_NEGOTIATION_END
};

UCP::UCP(Node *node, uint16_t requestedItemId, const AgentLocation &uccLocation, MCP* mcpParent) :
	Agent(node),
	_requestedItemId(requestedItemId),
	_uccLocation(uccLocation),
	_mcpParent(mcpParent),
	_negotiationAgreement(false)
{
	setState(ST_INIT);
}

UCP::~UCP()
{
}

void UCP::update()
{
	switch (state())
	{
	case ST_INIT:
		requestItem();
		setState(ST_OFFER_REQUEST);
		break;
	// TODO 2: Handle other states
	case ST_SEARCHING_CONSTRAINT:
		if (_mcp->negotiationFinished()) {
			sendConstraint(_mcp->requestedItemId());
			setState(ST_NEGOTIATION_END);
			_negotiationAgreement = _mcp->negotiationAgreement();
			destroyChildMCP();
		}
		break;
	default:;
	}
}

void UCP::finalize()
{
	destroyChildMCP();
	finish();
}

void UCP::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	PacketType packetType = packetHeader.packetType;
	// TODO 3: Handle requests
	if (state() == ST_OFFER_REQUEST && packetType == PacketType::ConstraintAcceptanceRequest) {
		
		// Read the packet
		PacketConstraintAcceptanceRequest packetData;
		packetData.Read(stream);

		if (packetData.constraintID == NULL_ITEM_ID) {
			setState(ST_NEGOTIATION_END);
			_negotiationAgreement = true;
		}
		else if (node()->HasItem(packetData.constraintID)) {
			setState(ST_NEGOTIATION_END);
			_negotiationAgreement = true;
			_mcpParent->setContributedItemId(packetData.constraintID);
		}
		else {
			createChildMCP(packetData.constraintID);
			setState(ST_SEARCHING_CONSTRAINT);
		}
	}
}

bool UCP::negotiationFinished() const {
	// TODO 4
	bool answer = state() == ST_NEGOTIATION_END;
	return answer;
}

bool UCP::negotiationAgreement() const {
	// TODO 5
	return _negotiationAgreement;
}


void UCP::requestItem()
{
	// TODO 6
	// Create message header and data
	PacketHeader packetHead;
	packetHead.packetType = PacketType::OfferRequest;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = _uccLocation.agentId;
	PacketOfferRequest packetData;
	packetData.offerID = _requestedItemId;

	// Serialize message
	OutputMemoryStream stream;
	packetHead.Write(stream);
	packetData.Write(stream);

	sendPacketToHost(_uccLocation.hostIP, _uccLocation.hostPort, stream);
}

void UCP::sendConstraint(uint16_t constraintItemId)
{
	// TODO 7
	// Create message header and data
	PacketHeader packetHead;
	packetHead.packetType = PacketType::ConstraintAcceptanceAnswer;
	packetHead.srcAgentId = id();
	packetHead.dstAgentId = _uccLocation.agentId;
	PacketConstraintAcceptanceAnswer packetData;
	packetData.constraintID = _requestedItemId;

	// Serialize message
	OutputMemoryStream stream;
	packetHead.Write(stream);
	packetData.Write(stream);

	sendPacketToHost(_uccLocation.hostIP, _uccLocation.hostPort, stream);
}

void UCP::createChildMCP(uint16_t constraintItemId)
{
	// TODO 8
	destroyChildMCP();
	_mcp.reset(new MCP(node(), constraintItemId));
	g_AgentContainer->addAgent(_mcp);
}

void UCP::destroyChildMCP()
{
	// TODO 9
	if (_mcp != nullptr) {
		_mcp->finalize();
		_mcp.reset();
	}
}
