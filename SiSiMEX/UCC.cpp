#include "UCC.h"



enum State
{
	// TODO 1: Add some states
	ST_NEGOTIATION_START,
	ST_WAITING_CONSTRAINT,
	ST_NEGOTIATION_END
};

UCC::UCC(Node *node, uint16_t contributedItemId, uint16_t constraintItemId) :
	Agent(node),
	_contributedItemId(contributedItemId),
	_constraintItemId(constraintItemId),
	_negotiationAgreement(false)
{
	setState(ST_NEGOTIATION_START);
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
	if (state() == ST_NEGOTIATION_START && packetType == PacketType::OfferRequest) {

	}
}

bool UCC::negotiationFinished() const {
	// TODO 3
	bool answer = state() == ST_NEGOTIATION_END;
	return answer;
}

bool UCC::negotiationAgreement() const {
	// TODO 4
	return _negotiationAgreement;
}
