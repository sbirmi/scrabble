# Word Check Feature Specification

## Overview

This specification defines a new feature allowing players to verify word validity during gameplay using the `!check <WORD>` command. This feature provides instant feedback on whether a word exists in the game's dictionary without affecting game state.

## Feature Requirements

### Functional Requirements

1. **Command Format**: Players can send `!check <WORD>` where:
   - `!check` can be in any case (e.g., `!CHECK`, `!Check`, `!check`)
   - `<WORD>` can be in any case (e.g., `HELLO`, `hello`, `Hello`)
   - Only one word is permitted after the command
   - Extra whitespace should be handled gracefully

2. **Word Validation**: The system validates the word against the same wordlist used for game moves

3. **Response**: Users receive immediate feedback on word validity

4. **Access Control**: Only active players and viewers in a game can use this command

### Non-Functional Requirements

1. **Performance**: Word checks should complete within 100ms
2. **Isolation**: Word checks do not affect game state, turn order, or scoring
3. **Rate Limiting**: No rate limiting is required
4. **Case Insensitive**: All processing should be case-insensitive

## High-Level Architecture

### Component Overview

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Web Client    │    │  Game Server    │    │   WordList      │
│   (JavaScript)  │    │   (C++)         │    │   Component     │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │ WEBSOCKET_CHECK       │                       │
         │──────────────────────▶│                       │
         │                       │ validateWord()        │
         │                       │──────────────────────▶│
         │                       │                       │
         │                       │ ◀──────────────────────│
         │ CHECK_RESULT          │   bool result          │
         │◀──────────────────────│                       │
```

### Component Responsibilities

#### 1. Web Client (JavaScript)
- **Input Processing**: Parse user input to detect `!check` commands
- **Message Formatting**: Create properly formatted WebSocket messages
- **UI Updates**: Display word check results to the user
- **Input Validation**: Basic client-side validation before sending to server

#### 2. Game Server (C++)
- **Command Processing**: Parse and validate incoming `!check` commands
- **Access Control**: Verify user permissions to use the feature
- **Word Validation**: Interface with WordList component for word checking
- **Response Generation**: Create appropriate JSON responses
- **Rate Limiting**: No rate limiting is required for this feature

#### 3. WordList Component (Existing)
- **Word Lookup**: Provide case-insensitive word validation
- **Dictionary Interface**: Maintain existing API for consistency

## Communication Protocol

### Request Message Format

**Client to Server (WebSocket)**
```json
["CHECK", "example"]
```

**Message Fields:**
- First element: Always "CHECK" (case-insensitive on input processing)
- Second element: The word to validate (string, case-insensitive)

### Response Message Format

**Server to Client (WebSocket)**

#### Success Response
```json
["CHECK-OKAY", "example", true, "✓ 'EXAMPLE' is a valid word"]
```

#### Invalid Word Response
```json
["CHECK-OKAY", "fakword", false, "✗ 'FAKWORD' is not a valid word"]
```

#### Error Response
```json
["CHECK-BAD", "Invalid input"]
```


### Error Conditions

1. **Invalid Format**
   - Multiple words provided
   - Empty word
   - Command malformed

2. **Access Denied**
   - User not in a game
   - User not authorized

3. **System Error**
   - WordList unavailable
   - Internal server error

## Implementation Details

### Server-Side Processing Flow

1. **Message Reception**: WebSocket message received
2. **Command Parsing**: Extract and validate `!check` command
3. **Access Control**: Verify user is in game and authorized
4. **Rate Limiting**: No specific rate limiting is required for this feature
5. **Word Processing**: Normalize word (trim, uppercase)
6. **Dictionary Lookup**: Query WordList component
7. **Response Generation**: Create appropriate JSON response
8. **Message Dispatch**: Send response to requesting client

### Client-Side Processing Flow

1. **Input Detection**: Monitor chat input for `!check` pattern
2. **Command Extraction**: Parse command and word
3. **Validation**: Basic client-side checks
4. **Message Sending**: Send WebSocket request
5. **Response Handling**: Process server response
6. **UI Update**: Display result to user

### Integration Points

#### Game Server Integration
- **New Message Handler**: `process_cmd_check()`
- **Message Router**: Add CHECK command to existing dispatcher
- **Response System**: Utilize existing JSON response framework

#### WordList Integration
- **API Usage**: Leverage existing `WordList::isValidWord()` method
- **No Changes Required**: Use existing interface without modification

#### Client Integration
- **Chat System**: Extend existing chat input processing
- **UI Components**: Add result display to game interface
- **Message Handling**: Extend WebSocket message processor

## Security Considerations

1. **Input Sanitization**: Validate all input parameters
2. **Access Control**: Ensure only authorized users can check words
3. **Resource Protection**: Limit WordList access frequency

## Testing Strategy

### Unit Tests
- Command parsing and validation
- WordList integration
- Error handling scenarios

### Integration Tests
- End-to-end WebSocket communication
- Client-server message flow
- Game state isolation verification

### Performance Tests
- Concurrent user handling

### Manual Testing Setup
**IMPORTANT**: Before building the Docker test image, ensure wordlist.txt is copied to the scrabble directory:

```bash
# Copy wordlist from parent directory for Docker testing
cp /Users/sharad/dev/scrabble/wordlist.txt /Users/sharad/dev/scrabble/scrabble/wordlist.txt

# Build and run test container
docker build -f Dockerfile-test -t scrabble-test .
docker run -p 5050:5050 -p 5051:5051 scrabble-test
```

The wordlist.txt file is required for proper word validation during testing.

## Future Enhancements

1. **Batch Checking**: Allow checking multiple words
2. **Definition Lookup**: Provide word definitions
3. **Usage Statistics**: Track most-checked words
4. **Pronunciation Guide**: Audio pronunciation support

## Backward Compatibility

This feature is purely additive and maintains full backward compatibility:
- No changes to existing game mechanics
- No modifications to existing message formats
- No impact on current user workflows