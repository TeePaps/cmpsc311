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

// Defines

// Functional Prototypes

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
  int i;
  for (i = 0; i < SMSA_DISK_ARRAY_SIZE; i++) {
    smsa_operation( smsa_get_instruction( SMSA_SEEK_DRUM, i, 0) );
    smsa_operation( smsa_get_instruction( FORMAT_DRUM, i, 0 ) );
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_vunmount
// Description  :  Unmount the SMSA disk array virtual address space
//
// Inputs       : none
// Outputs      : -1 if failure or 0 if successful

int smsa_vunmount( void )  {

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

int smsa_vread( SMSA_VIRTUAL_ADDRESS addr, uint32_t len, unsigned char *buf ) {

  if ( addr >= ( SMSA_DISK_SIZE * SMSA_DISK_ARRAY ) ) {
    logMessage( BAD_DRUM_ID, "Address for read is out of range");
    return -1;
  };

  // Initialize data
  boolean firstBlock = true;
  char temp[( SMSA_DISK_SIZE / SMSA_BLOCK_SIZE )]; // temporary byte buffer
  unsigned int = readBytes = 0;
  SMSA_DRUM_SIZE drum = get_drum_id( addr );
  SMSA_BLOCK_SIZE block = get_block_id( addr );
  SMSA_OFFSET_SIZE offset = get_offset( addr );

  // Loop through as many drums as necessary
  do {
    smsa_operation( smsa_get_instruction( SMSA_SEEK_DRUM, drum, 0 ), temp );

    // Loop through as many blocks as necessary
    do {
      smsa_operation( smsa_get_instruction( SMSA_SEEK_BLOCK, drum, block ), temp );
      smsa_operation( smsa_get_instruction( SMSA_DISK_READ, drum, block ), temp );
      read( len, offset, firstBlock, readBytes, temp, buf );
      firstBlock = false;
      block++;

    } while ( ( readBytes < len ) && ( block < SMSA_MAX_BLOCK_ID ) );
    drum++;

  } while ( readBytes < len );

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

int smsa_vwrite( SMSA_VIRTUAL_ADDRESS addr, uint32_t len, unsigned char *buf )  {

}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : get_drum_id
// Description  : Get the drum id of a given address
//
// Inputs       : addr - the address to write to
// Outputs      : -1 if failure or the drum id if successful

SMSA_DRUM_SIZE get_drum_id ( SMSA_VIRTUAL_ADDRESS addr ) {
  return( addr >> 16 );
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : get_block_id
// Description  : Get the block id of a given address
//
// Inputs       : addr - the address to write to
// Outputs      : -1 if failure or the drum id if successful

SMSA_BLOCK_SIZE get_block_id ( SMSA_VIRTUAL_ADDRESS addr ) {
  return( ( addr & 0xffff ) >> 8 ); 
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : get_offset
// Description  : Get the block id of a given address
//
// Inputs       : addr - the address to write to
// Outputs      : -1 if failure or the drum id if successful

SMSA_BLOCK_SIZE get_offset ( SMSA_VIRTUAL_ADDRESS addr ) {
  return( addr & 0xff ); 
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_get_instruction
// Description  : Get the drum id of a given address
//
// Inputs       : addr - the address to write to
// Outputs      : -1 if failure or the drum id if successful

uint32_t get_instruction ( SMSA_DISK_COMMAND opcode, SMSA_DRUM_SIZE drumId, SMSA_BLOCK_ID blockId ) {
    uint32_t instruction = opcode;
    instruction <<= 4; // make room for drumId;
    instruction |= drumId;
    instruction <<= 22; // make room for blockId;
    instruction |= blockId;

    return( instruction );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : smsa_read_until_block_end
// Description  : Get the drum id of a given address
//
// Inputs       : addr - the address to write to
// Outputs      : -1 if failure or the drum id if successful

void read( uint32_t len, SMSA_OFFSET_SIZE offset, boolean firstBlock, int& readBytes, char* temp, char* buf ) {
  if (firstBlock) {
    i = offset;
  }
  else {
    i = 0;
  }

  do {
    buf[readBytes] = temp[i];
    readBytes++;
    i++;
     
  } while( i < SMSA_OFFSET_SIZE && readBytes < len );

}


