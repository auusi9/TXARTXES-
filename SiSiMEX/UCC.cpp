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
		PacketOfferRequest packetData;
		packetData.Read(stream);
		
		if (packetData.offerID == _contributedItemId) {
			// Create message header and data
			PacketHeader packetHead;
			packetHead.packetType = PacketType::ConstraintAcceptanceRequest;
			packetHead.srcAgentId = id();
			packetHead.dstAgentId = packetHeader.srcAgentId;
			PacketConstraintAcceptanceRequest packetData;
			packetData.constraintID = _constraintItemId;

			// Serialize message
			OutputMemoryStream stream;
			packetHead.Write(stream);
			packetData.Write(stream);

			// When do we have to use "SendPacket()" over "sendPacketToHost(hostIP, port, stream)" ?
			socket->SendPacket(stream.GetBufferPtr(), stream.GetSize());

			if (_constraintItemId == NULL_ITEM_ID) {
				setState(ST_NEGOTIATION_END);
				_negotiationAgreement = true;
			}
			else {
				setState(ST_WAITING_CONSTRAINT);
			}
		}
		else {
			// ERROR (dunno if this could ever happen)
		}
	}
	else if (state() == ST_WAITING_CONSTRAINT && packetType == PacketType::ConstraintAcceptanceAnswer) {
		PacketConstraintAcceptanceAnswer packetData;
		packetData.Read(stream);

		if (packetData.constraintID == _constraintItemId) {
			// Create message header and data

			setState(ST_NEGOTIATION_END);
			_negotiationAgreement = true;
		}
		else {
			// ERROR (dunno if this could ever happen)
		}
	}
}

bool UCC::negotiationFinished() const {
	// TODO 3
	return state() == ST_NEGOTIATION_END;
}

bool UCC::negotiationAgreement() const {
	// TODO 4
	return _negotiationAgreement;
}
