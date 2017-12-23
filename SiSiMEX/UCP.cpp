#include "UCP.h"
#include "MCP.h"
#include "AgentContainer.h"


enum State
{
	ST_INIT,
	// TODO 1: Add other states...
};

UCP::UCP(Node *node, uint16_t requestedItemId, const AgentLocation &uccLocation) :
	Agent(node),
	_requestedItemId(requestedItemId),
	_uccLocation(uccLocation),
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
		//setState(ST_WHATEVER_NEXT_STATE...);
		break;

	// TODO 2: Handle other states

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
}

bool UCP::negotiationFinished() const {
	// TODO 4
	return false;
}

bool UCP::negotiationAgreement() const {
	// TODO 5
	return false;
}


void UCP::requestItem()
{
	// TODO 6

	//sendPacketToHost(ip, port, ostream);
}

void UCP::sendConstraint(uint16_t constraintItemId)
{
	// TODO 7

	//sendPacketToHost(ip, port, ostream);
}

void UCP::createChildMCP(uint16_t constraintItemId)
{
	// TODO 8
}

void UCP::destroyChildMCP()
{
	// TODO 9
}
