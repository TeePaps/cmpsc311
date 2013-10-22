////////////////////////////////////////////////////////////////////////////////
//
//  File           : smsa_driver.c
//  Description    : This is the driver for the SMSA simulator.
//
//   Author        : 
//   Last Modified : 
//

// Include Files

// Project Include Files
#include <smsa_driver.h>
#include <cmpsc311_log.h>
#include <assert.h>

// Defines

// Functional Prototypes
bool valid_address( uint32_t addr );
SMSA_DRUM_ID get_drum_id( uint32_t addr );
SMSA_BLOCK_ID get_block_id( uint32_t addr );
SMSA_BLOCK_ID get_offset( uint32_t addr );
uint32_t get_instruction( SMSA_DISK_COMMAND opcode, SMSA_DRUM_ID drumId, SMSA_BLOCK_ID blockId );
void read_buf( uint32_t len, SMSA_BLOCK_ID offset, bool firstBlock, int* readBytes, unsigned char* temp, unsigned char* buf );
void write_buf( uint32_t len, SMSA_BLOCK_ID offset, bool firstBlock, int* writtenBytes, unsigned char* temp, unsigned char* buf );

//
// Global data

// Interfaces

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_vmount
// Description  : Mount the SMSA disk array virtual address space
//
// Inputs       : none
// Outputs      : -1 if failure or 0 if successful

int smsa_vmount( void ) {
  return( smsa_operation( get_instruction( SMSA_MOUNT, 0, 0 ), NULL ) );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_vunmount
// Description  :  Unmount the SMSA disk array virtual address space
//
// Inputs       : none
// Outputs      : -1 if failure or 0 if successful

int smsa_vunmount( void )  {
  return( smsa_operation( get_instruction( SMSA_UNMOUNT, 0, 0 ), NULL ) );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_vread
// Description  : Read from the SMSA virtual address space
//
// Inputs       : addr - the address to read from
//                len - the number of bytes to read
//                buf - the place to put the read bytes
// Outputs      : -1 if failure or 0 if successful

int smsa_vread( uint32_t addr, uint32_t len, unsigned char *buf ) {
  if ( !valid_address( addr ) ) {
    return -1;
  }

  // Initialize data
  bool firstBlock = true;
  unsigned char temp[SMSA_OFFSET_SIZE]; // temporary byte buffer
  int readBytes = 0;
  SMSA_DRUM_ID drum = get_drum_id( addr );
  SMSA_BLOCK_ID block = get_block_id( addr );
  SMSA_BLOCK_ID offset = get_offset( addr );

  // Loop through as many drums as necessary
  do {
    smsa_operation( get_instruction( SMSA_SEEK_DRUM, drum, block ), NULL );

    // Loop through as many blocks as necessary
    do {
      smsa_operation( get_instruction( SMSA_SEEK_BLOCK, drum, block ), NULL );
      smsa_operation( get_instruction( SMSA_DISK_READ, drum, block ), temp );
      read_buf( len, offset, firstBlock, &readBytes, temp, buf );
      firstBlock = false;
      block++;
    } while ( ( readBytes < len ) && ( block < SMSA_MAX_BLOCK_ID ) );
    
    drum++;
    block = 0;
  } while ( ( readBytes < len ) && ( drum < SMSA_DISK_ARRAY_SIZE ) );

  return 0;
}
  

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_vwrite
// Description  : Write to the SMSA virtual address space
//
// Inputs       : addr - the address to write to
//                len - the number of bytes to write
//                buf - the place to read the read from to write
// Outputs      : -1 if failure or 0 if successful

int smsa_vwrite( uint32_t addr, uint32_t len, unsigned char *buf )  {
  if ( !valid_address( addr ) ) {
    return -1;
  }

  // Initialize data
  bool firstBlock = true;
  unsigned char temp[SMSA_OFFSET_SIZE]; // temporary byte buffer
  int writtenBytes = 0;
  SMSA_DRUM_ID drum = get_drum_id( addr );
  SMSA_BLOCK_ID block = get_block_id( addr );
  SMSA_BLOCK_ID offset = get_offset( addr );

 printf("Inside block loop: drum = %d/\tblock = %d/\toffset = %d/\n", drum,block,offset);
  
  // Loop through as many drums as necessary
  do {
    smsa_operation( get_instruction( SMSA_SEEK_DRUM, drum, block ), NULL );

    // Loop through as many blocks as necessary
    do {
      printf("Inside block loop: drum = %d/\tblock = %d/\toffset = %d/\n", drum,block,offset);

      // Read data already present into temporary buffer then seek back to
      // start of block
      smsa_operation( get_instruction( SMSA_SEEK_BLOCK, drum, block ), NULL );
      smsa_operation( get_instruction( SMSA_DISK_READ, drum, block ), temp );
      smsa_operation( get_instruction( SMSA_SEEK_DRUM, drum, block ), NULL );
      smsa_operation( get_instruction( SMSA_SEEK_BLOCK, drum, block ), NULL );

      write_buf( len, offset, firstBlock, &writtenBytes, temp, buf );
      smsa_operation( get_instruction( SMSA_DISK_WRITE, drum, block), temp );
      firstBlock = false;
      block++;
    } while ( ( writtenBytes < len ) && ( block < SMSA_MAX_BLOCK_ID ) );

    drum++;
    block = 0;
  } while ( ( writtenBytes < len ) && ( drum < SMSA_DISK_ARRAY_SIZE ) );

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : valid_address
// Description  : Check that the given address is in the range of our device
//
// Inputs       : addr - the address to check
// Outputs      : true if in range, false if not
bool valid_address( uint32_t addr ) {
  if ( addr > MAX_SMSA_VIRTUAL_ADDRESS) {
    logMessage( SMSA_BAD_DRUM_ID, "Address for read is out of range");
    return false;
  }
  else {
    return true;
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : get_drum_id
// Description  : Get the drum id of a given address
//
// Inputs       : addr - the address to write to
// Outputs      : -1 if failure or the drum id if successful

SMSA_DRUM_ID get_drum_id ( uint32_t addr ) {
  return( addr >> 16 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : get_block_id
// Description  : Get the block id of a given address
//
// Inputs       : addr - the address to write to
// Outputs      : -1 if failure or the drum id if successful

SMSA_BLOCK_ID get_block_id ( uint32_t addr ) {
  return( ( addr & 0xffff ) >> 8 ); 
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : get_offset
// Description  : Get the block id of a given address
//
// Inputs       : addr - the address to write to
// Outputs      : -1 if failure or the drum id if successful

SMSA_BLOCK_ID get_offset ( uint32_t addr ) {
  return( addr & 0xff ); 
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_get_instruction
// Description  : Get the drum id of a given address
//
// Inputs       : addr - the address to write to
// Outputs      : -1 if failure or the drum id if successful

uint32_t get_instruction ( SMSA_DISK_COMMAND opcode, SMSA_DRUM_ID drumId, SMSA_BLOCK_ID blockId ) {
    uint32_t instruction = opcode;
    instruction <<= 4; // make room for drumId;
    instruction |= drumId;
    instruction <<= 22; // make room for blockId;
    instruction |= blockId;

    return( instruction );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : read
// Description  : Read the bytes from the temporary buffer into the buffer//
// Inputs       : len - 
//                offset -
//                firstBlock - checks if first block to account for offset
//                readBytes - a reference to the number of bytes already read
//                temp - the temporary buffer which stores the block data
//                buf - 
// Outputs      : -1 if failure or the drum id if successful

void read_buf( uint32_t len, SMSA_BLOCK_ID offset, bool firstBlock, int* readBytes, unsigned char* temp, unsigned char* buf ) {
  int i;

  if (firstBlock) {
    i = offset;
  }
  else {
    i = 0;
  }

  do {
    int index = *readBytes;
    buf[index] = temp[i];
    (*readBytes)++;
    i++;
  } while( ( i < SMSA_OFFSET_SIZE ) && ( *readBytes < len ) );
  assert(i <=256);

}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : write
// Description  : write the bytes given into the buffer
// Inputs       : len - 
//                offset -
//                firstBlock - checks if first block to account for offset
//                readBytes - a reference to the number of bytes already read
//                temp - the temporary buffer which stores the block data
//                buf - 
// Outputs      : -1 if failure or the drum id if successful


void write_buf( uint32_t len, SMSA_BLOCK_ID offset, bool firstBlock, int* writtenBytes, unsigned char* temp, unsigned char* buf ) {
  int i;

  if (firstBlock) {
    i = offset;
  }
  else {
    i = 0;
  }

  do {
    int index = *writtenBytes;
    temp[i] = buf[index];
    (*writtenBytes)++;
    i++;
     
  } while( ( i < SMSA_OFFSET_SIZE ) && ( *writtenBytes < len ) );

}

