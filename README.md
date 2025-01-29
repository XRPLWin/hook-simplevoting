# Simple Voting Contract Hook
Simple Voting Contract Hook for Xahau Network

## Installation
Hook is installed on one account, users send `Invoke` transaction to that account to vote for one of candidates.

HookOn triggers: `ttINVOKE`

## Setup
To add new candidate send `Invoke` transaction from account which have this hook installed (self).

```
{
    "TransactionType": "Invoke",
    "Account": "rVotingaccount... (self)",
    "HookParameters": [
      {
        "HookParameter": {
          "HookParameterName": <uint8ToHex(0)>,
          "HookParameterValue": <uint16ToHex(1)>+<(convertStringToHex('Name of candidate 1').padEnd(64,'0'))>
        }
      },
      {
        "HookParameter": {
          "HookParameterName": <uint8ToHex(1)>,
          "HookParameterValue": <uint16ToHex(6)>+<(convertStringToHex('Name of candidate 6').padEnd(64,'0'))>
        }
      },
      ...
...
```
In each transaction HookParameterName starts with 0, this ensures hook treats parameters as array-like structure. 
HookParameterValue is coumpound data of UInt16 number in HEX - that is candidate ID, followed by candidate title (32 chars max).  
Once executed, hookstate row will be added.


## Voting
To vote on existing candidate ID, send Invoke transaction to account which have this hook installed.
```
{
    "TransactionType": "Invoke",
    "Account": "rVoter...",
    "HookParameters": [
      {
        "HookParameter": {
          "HookParameterName": <convertStringToHex("KEY")>,
          "HookParameterValue": <uint16ToHex(1)>
        }
      },
    ],
    "Destination": "rVotingaccount...",
```
HookParameterName - always "KEY"
HookParameterValue - candidate ID in this sample "Name of candidate 1"

## ABI for hookstates
```JSON
{ 
  "name": "votinghook",
  "hook_account": "",
  "hook_namespace_id": "",
  "hook_definition": {
    "name": "votinghook",
    "description": "votinghook",
    "hook_hash": "",
    "version": [],
    "hook_states": {
      "name": "votinghook",
      "description": "votinghook",
      "fields": [
        {
          "name": "Candidates",
          "hookstate_key": [
            {
              "type": "Null",
              "name": "padding",
              "byte_length": 31,
              "exclude": true
            },
            {
              "type": "UInt8",
              "name": "ID",
              "exclude": false
            }
          ],
          "hookstate_data": [
            {
              "type": "VarString",
              "name": "Candidate name",
              "byte_length":32,
              "exclude":false
            },
            {
              "type":"UInt16",
              "name":"Number of votes"
            }
          ]
        },
        {
          "name": "Votes",
          "hookstate_key": [
            {
              "type": "Null",
              "name": "padding",
              "byte_length": 12,
              "exclude": true
            },
            {
              "type": "AccountID",
              "name": "Voter"
            }
          ],
          "hookstate_data": [
            {
              "type": "VarString",
              "name": "Candidate name",
              "byte_length":32,
              "exclude":false
            }
          ]
        }
      ]
    }
  }
}
```
