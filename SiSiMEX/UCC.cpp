#include "UCC.h"



enum State
{
	// TODO 1: Add some states
};

UCC::UCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId) :
	Agent(node),
	_contributedItemId(contributedItemId),
	_constraintItemId(constraintItemId),
	_negotiationAgreement(false)
{
	//setState(ST_WHATEVER_INITIAL_STATE ...);
}

UCC::~UCC()
{
}

void UCC::update()
{
	// Nothing to do
}

void UCC::finalize()
{
	finish();
}

void UCC::OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream)
{
	PacketType packetType = packetHeader.packetType;

	// TODO 2 Receive requests and send back responses...
}

bool UCC::negotiationFinished() const {
	// TODO 3
	return false;
}

bool UCC::negotiationAgreement() const {
	// TODO 4
	return false;
}
