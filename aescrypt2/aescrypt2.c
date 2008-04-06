/*
 *  AES-CBC-128/HMAC-MD5 file encryption, v2
 *
 *  Copyright (C) 2001-2004  Christophe Devine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "aes.h"
#include "md5.h"

#define MODE_ENCRYPT    0
#define MODE_DECRYPT    1

/* miscellaneous utility routines */

int my_fread( FILE *f, void *ptr, size_t size )
{
    if( fread( ptr, 1, size, f ) != size )
    {
        if( ferror( f ) )
        {
            fprintf( stderr, "fread(%d bytes) failed\n", size );
            return( 1 );
        }

        if( feof( f ) )
        {
            fprintf( stderr, "fread(%d bytes): short file\n", size );
            return( 1 );
        }
    }

    return( 0 );
}

int my_fwrite( FILE *f, void *ptr, size_t size )
{
    if( fwrite( ptr, 1, size, f ) != size )
    {
        if( ferror( f ) )
        {
            fprintf( stderr, "fwrite(%d bytes) failed\n", size );
            return( 1 );
        }

        if( feof( f ) )
        {
            fprintf( stderr, "fwrite(%d bytes): short file\n", size );
            return( 1 );
        }
    }

    return( 0 );
}

/* program entry point */

int main( int argc, char *argv[] )
{
    char key[256];
    unsigned char IV[16];
    unsigned char tmp[16];
    unsigned char digest[16];
    unsigned char k_ipad[64];
    unsigned char k_opad[64];
    unsigned char buffer[1024];
    long int filesize, offset;
    int i, n, mode, lastn;
    FILE *fkey, *fin, *fout;
    aes_context aes_ctx;
    md5_context md5_ctx;

    /* check the arguments */

    if( argc != 5)
    {
        printf( "\n  usage: aescrypt2 <mode> <infile> <outfile> "
                "<key file || key string>\n\n   mode: %d = encrypt,"
                " %d = decrypt.\n\n", MODE_ENCRYPT, MODE_DECRYPT );
#ifdef WIN32
        printf( "  Press Ctrl-C to exit.\n" );
        scanf( "%d", &i );
#endif
        return( 1 );
    }

    sscanf( argv[1], "%d", &mode );

    if( mode != MODE_ENCRYPT && mode != MODE_DECRYPT )
    {
        fprintf( stderr, "invalide mode \"%d\"\n", mode );
        return( 1 );
    }

    if( ! ( fin = fopen( argv[2], "rb" ) ) )
    {
        fprintf( stderr, "fopen(%s,rb) failed\n", argv[2] );
        return( 1 );
    }

    /* read the secret key and clean the command line */

    memset( key, 0, sizeof( key ) );

    if( ( fkey = fopen( argv[4], "rb" ) ) != NULL )
    {
        fread( key, 1, sizeof( key ) - 1, fkey );
        fclose( fkey );
    }
    else
        strncpy( key, argv[4], sizeof( key ) - 1 );

    memset( argv[4], 0, strlen( argv[4] ) );

    /* read the input file size */

    if( fseek( fin, 0, SEEK_END ) < 0 )
    {
        fprintf( stderr, "fseek(SEEK_END) failed\n" );
        return( 1 );
    }

    filesize = ftell( fin );

    if( fseek( fin, 0, SEEK_SET ) < 0 )
    {
        fprintf( stderr, "fseek(SEEK_SET) failed\n" );
        return( 1 );
    }

    if( mode == MODE_ENCRYPT )
    {
        /* compute the initialization vector: *
         * IV = md5sum( filesize + filename ) */

        buffer[0] =   filesize         & 0xFF;
        buffer[1] = ( filesize <<  8 ) & 0xFF;
        buffer[2] = ( filesize << 16 ) & 0xFF;
        buffer[3] = ( filesize << 24 ) & 0xFF;

        md5_starts( &md5_ctx );
        md5_update( &md5_ctx, buffer, 4 );
        md5_update( &md5_ctx, (unsigned char *) argv[2],
                                        strlen( argv[2] ) );
        md5_finish( &md5_ctx, IV );

        lastn = filesize & 0x0F;

        IV[15] &= 0xF0;
        IV[15] |= lastn;

        /* attempt to create the output file */

        if( ! ( fout = fopen( argv[3], "wb" ) ) )
        {
            fprintf( stderr, "fopen(%s,wb) failed\n", argv[3] );
            return( 1 );
        }

        /* append the IV at the beginning of the output */

        if( my_fwrite( fout, IV, 16 ) )
            return( 1 );

        /* hash the IV and the secret key together,  using the *
         * result to set the AES-CBC-128 and HMAC-MD5 contexts */

        md5_starts( &md5_ctx );
        md5_update( &md5_ctx, IV, 16 );
        md5_update( &md5_ctx, (unsigned char *) key,
                                        strlen( key ) );
        md5_finish( &md5_ctx, digest );

        aes_set_key( &aes_ctx, digest, 128 );

        memset( k_ipad, 0x36, 64 );
        memset( k_opad, 0x5C, 64 );

        for( i = 0; i < 16; i++ )
        {
            k_ipad[i] ^= digest[i];
            k_opad[i] ^= digest[i];
        }

        /* the secret key can be blanked now */

        memset( key, 0, sizeof( key ) );

        /* encrypt and write the ciphertext */

        md5_starts( &md5_ctx );
        md5_update( &md5_ctx, k_ipad, 64 );

        for( offset = 0; offset < filesize; offset += 16 )
        {
            n = ( filesize - offset > 16 ) ? 16 :
                  filesize - offset;

            if( my_fread( fin, buffer, n ) )
                return( 1 );

            for( i = 0; i < 16; i++ )
                buffer[i] ^= IV[i];

            aes_encrypt( &aes_ctx, buffer, buffer );
            md5_update( &md5_ctx, buffer, 16 );
            memcpy( IV, buffer, 16 );

            if( my_fwrite( fout, buffer, 16 ) )
                return( 1 );
        }

        if( ! filesize )
        {
            memcpy( buffer, IV, 16 );
            aes_encrypt( &aes_ctx, buffer, buffer );
            md5_update( &md5_ctx, buffer, 16 );
        }

        /* finally write the HMAC */

        md5_finish( &md5_ctx, digest );

        md5_starts( &md5_ctx );
        md5_update( &md5_ctx, k_opad, 64 );
        md5_update( &md5_ctx, digest, 16 );
        md5_finish( &md5_ctx, digest );

        if( my_fwrite( fout, digest, 16 ) )
            return( 1 );
    }

    if( mode == MODE_DECRYPT )
    {
        /* check the file size */

        if( filesize < 32 )
        {
            fprintf( stderr, "file too short to be encrypted!\n" );
            return( 1 );
        }

        if( filesize & 0x0F )
        {
            fprintf( stderr, "file size not a multiple of 16!\n" );
            return( 1 );
        }

        /* read the IV and # of last n bytes */

        if( my_fread( fin, buffer, 16 ) )
            return( 1 );

        memcpy( IV, buffer, 16 );

        lastn = IV[15] & 0x0F;

        /* hash the IV and the secret key together, using the *
         * result to setup the AES-128 and HMAC-MD5 contexts  */

        md5_starts( &md5_ctx );
        md5_update( &md5_ctx, IV, 16 );
        md5_update( &md5_ctx, (unsigned char *) key,
                                        strlen( key ) );
        md5_finish( &md5_ctx, digest );

        aes_set_key( &aes_ctx, digest, 128 );

        memset( k_ipad, 0x36, 64 );
        memset( k_opad, 0x5C, 64 );

        for( i = 0; i < 16; i++ )
        {
            k_ipad[i] ^= digest[i];
            k_opad[i] ^= digest[i];
        }

        /* the secret key can be blanked now */

        memset( key, 0, sizeof( key ) );

        /* first of all, verify the HMAC */

        filesize -= 32;

        md5_starts( &md5_ctx );
        md5_update( &md5_ctx, k_ipad, 64 );

        for( offset = 0; offset < filesize; offset += 1024 )
        {
            n = ( filesize - offset > 1024 ) ? 1024 :
                  filesize - offset;

            if( my_fread( fin, buffer, n ) )
                return( 1 );

            md5_update( &md5_ctx, buffer, n );
        }

        if( ! filesize )
        {
            memcpy( buffer, IV, 16 );
            aes_encrypt( &aes_ctx, buffer, buffer );
            md5_update( &md5_ctx, buffer, 16 );
        }

        md5_finish( &md5_ctx, digest );
        md5_starts( &md5_ctx );
        md5_update( &md5_ctx, k_opad, 64 );
        md5_update( &md5_ctx, digest, 16 );
        md5_finish( &md5_ctx, digest );

        if( my_fread( fin, buffer, 16 ) )
            return( 1 );

        if( memcmp( digest, buffer, 16 ) )
        {
            fprintf( stderr, "HMAC check failed (wrong key ?)\n" );
            return( 1 );
        }

        /* attempt to create the output file */

        if( ! ( fout = fopen( argv[3], "wb" ) ) )
        {
            fprintf( stderr, "fopen(%s,wb) failed\n", argv[3] );
            return( 1 );
        }

        /* now decrypt and write the plaintext */

        if( fseek( fin, 16, SEEK_SET ) < 0 )
        {
            fprintf( stderr, "fseek(SEEK_SET) failed\n" );
            return( 1 );
        }

        for( offset = 0; offset < filesize; offset += 16 )
        {
            if( my_fread( fin, buffer, 16 ) )
                return( 1 );

            memcpy( tmp, buffer, 16 );
 
            aes_decrypt( &aes_ctx, buffer, buffer );
   
            for( i = 0; i < 16; i++ )
                buffer[i] ^= IV[i];

            memcpy( IV, tmp, 16 );

            n = ( lastn > 0 && offset == filesize - 16 )
                ? lastn : 16;

            if( my_fwrite( fout, buffer, n ) )
                return( 1 );
        }
    }

    return( 0 );
}
