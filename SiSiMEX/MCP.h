#pragma once
#include "Agent.h"

// Forward declaration
class UCP;
using UCPPtr = std::shared_ptr<UCP>;

class MCP :
	public Agent
{
public:

	// Constructor and destructor
	MCP(Node *node, uint16_t itemId);
	~MCP();

	// Getters
	uint16_t requestedItemId() const { return _itemId; }
	uint16_t contributedItemId() const { return _contributedItemId; }

	// Setters
	void setContributedItemId(uint16_t itemId) { _contributedItemId = itemId; }

	void update() override;
	void finalize() override;

	void OnPacketReceived(TCPSocketPtr socket, const PacketHeader &packetHeader, InputMemoryStream &stream) override;

	// Whether or not the negotiation finished
	bool negotiationFinished() const;

	// Whether or not there was a negotiation agreement
	bool negotiationAgreement() const;

private:

	bool queryMCCsForItem(int itemId);

	bool sendNegotiationRequest(const AgentLocation &mccRegister);

	void createChildUCP(const AgentLocation &uccLoc);

	void destroyChildUCP();

	void selectMCC();

	uint16_t _itemId;

	uint16_t _contributedItemId;

	int _mccRegisterIndex; /**< Iterator through _mccRegisters. */
	std::vector<AgentLocation> _mccRegisters; /**< MCCs returned by the YP. */

	UCPPtr _ucp; /**< Child UCP. */

	bool _negotiationAgreement; /**< Was there a negotiation agreement? */
};

