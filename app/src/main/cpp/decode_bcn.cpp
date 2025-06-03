

// =========================== COMPRESSION ============================
// Some texture has block compression, when uncompressed will have swizzled layout. Since on some backend, no
// option is provided to make the GPU driver not try to translate the layout to linear, we have to do uncompress
// and unswizzled on the CPU.

// This BC decompression code is based on code from AMD GPUOpen's Compressonator

/**
 * \brief Decompresses one block of a BC1 texture and stores the resulting pixels at the appropriate offset in 'image'.
 *
 * \param block_storage     pointer to the block to decompress.
 * \param image             pointer to image where the decompressed pixel data should be stored.
 **/
void decompress_block_bc1(const uint8_t *block_storage, uint32_t *image) {
    std::uint16_t n0 = static_cast<std::uint16_t>((block_storage[1] << 8) | block_storage[0]);
    std::uint16_t n1 = static_cast<std::uint16_t>((block_storage[3] << 8) | block_storage[2]);

    block_storage += 4;

    std::uint8_t r0 = (n0 & 0xF800) >> 8;
    std::uint8_t g0 = (n0 & 0x07E0) >> 3;
    std::uint8_t b0 = (n0 & 0x001F) << 3;

    std::uint8_t r1 = (n1 & 0xF800) >> 8;
    std::uint8_t g1 = (n1 & 0x07E0) >> 3;
    std::uint8_t b1 = (n1 & 0x001F) << 3;

    r0 |= r0 >> 5;
    r1 |= r1 >> 5;
    g0 |= g0 >> 6;
    g1 |= g1 >> 6;
    b0 |= b0 >> 5;
    b1 |= b1 >> 5;

    std::uint32_t c0 = 0xFF000000 | (b0 << 16) | (g0 << 8) | r0;
    std::uint32_t c1 = 0xFF000000 | (b1 << 16) | (g1 << 8) | r1;

    if (n0 > n1) {
        std::uint8_t r2 = static_cast<uint8_t>((2 * r0 + r1 + 1) / 3);
        std::uint8_t r3 = static_cast<uint8_t>((2 * r1 + r0 + 1) / 3);
        std::uint8_t g2 = static_cast<uint8_t>((2 * g0 + g1 + 1) / 3);
        std::uint8_t g3 = static_cast<uint8_t>((2 * g1 + g0 + 1) / 3);
        std::uint8_t b2 = static_cast<uint8_t>((2 * b0 + b1 + 1) / 3);
        std::uint8_t b3 = static_cast<uint8_t>((2 * b1 + b0 + 1) / 3);

        std::uint32_t c2 = 0xFF000000 | (b2 << 16) | (g2 << 8) | r2;
        std::uint32_t c3 = 0xFF000000 | (b3 << 16) | (g3 << 8) | r3;

        for (int i = 0; i < 16; ++i) {
            int index = (block_storage[i / 4] >> (i % 4 * 2)) & 0x03;
            switch (index) {
            case 0:
                image[i] = c0;
                break;
            case 1:
                image[i] = c1;
                break;
            case 2:
                image[i] = c2;
                break;
            case 3:
                image[i] = c3;
                break;
            }
        }
    } else {
        // Transparent decode
        std::uint8_t r2 = static_cast<uint8_t>((r0 + r1) / 2);
        std::uint8_t g2 = static_cast<uint8_t>((g0 + g1) / 2);
        std::uint8_t b2 = static_cast<uint8_t>((b0 + b1) / 2);

        std::uint32_t c2 = 0xFF000000 | (b2 << 16) | (g2 << 8) | r2;

        for (int i = 0; i < 16; ++i) {
            int index = (block_storage[i / 4] >> (i % 4 * 2)) & 0x03;
            switch (index) {
            case 0:
                image[i] = c0;
                break;
            case 1:
                image[i] = c1;
                break;
            case 2:
                image[i] = c2;
                break;
            case 3:
                image[i] = 0x00000000;
                break;
            }
        }
    }
}

/**
 * \brief Decompresses one block of a alpha texture and stores the resulting pixels at the appropriate offset in 'image'.
 *
 * \param block_storage     pointer to the block to decompress.
 * \param image             pointer to image where the decompressed pixel data should be stored.
 * \param offset            offset to where data should be written.
 * \param stride            stride between bytes to where data should be written.
 **/
void decompress_block_alpha(const uint8_t *block_storage, uint8_t *image, const uint32_t offset, const uint32_t stride) {
    uint8_t alpha[8];

    alpha[0] = block_storage[0];
    alpha[1] = block_storage[1];

    if (alpha[0] > alpha[1]) {
        // 8-alpha block:  derive the other six alphas.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = static_cast<uint8_t>((6 * alpha[0] + 1 * alpha[1] + 3) / 7); // bit code 010
        alpha[3] = static_cast<uint8_t>((5 * alpha[0] + 2 * alpha[1] + 3) / 7); // bit code 011
        alpha[4] = static_cast<uint8_t>((4 * alpha[0] + 3 * alpha[1] + 3) / 7); // bit code 100
        alpha[5] = static_cast<uint8_t>((3 * alpha[0] + 4 * alpha[1] + 3) / 7); // bit code 101
        alpha[6] = static_cast<uint8_t>((2 * alpha[0] + 5 * alpha[1] + 3) / 7); // bit code 110
        alpha[7] = static_cast<uint8_t>((1 * alpha[0] + 6 * alpha[1] + 3) / 7); // bit code 111
    } else {
        // 6-alpha block.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = static_cast<uint8_t>((4 * alpha[0] + 1 * alpha[1] + 2) / 5); // Bit code 010
        alpha[3] = static_cast<uint8_t>((3 * alpha[0] + 2 * alpha[1] + 2) / 5); // Bit code 011
        alpha[4] = static_cast<uint8_t>((2 * alpha[0] + 3 * alpha[1] + 2) / 5); // Bit code 100
        alpha[5] = static_cast<uint8_t>((1 * alpha[0] + 4 * alpha[1] + 2) / 5); // Bit code 101
        alpha[6] = 0; // Bit code 110
        alpha[7] = 255; // Bit code 111
    }

    image += offset;

    image[stride * 0] = alpha[block_storage[2] & 0x07];
    image[stride * 1] = alpha[(block_storage[2] >> 3) & 0x07];
    image[stride * 2] = alpha[((block_storage[3] << 2) & 0x04) | ((block_storage[2] >> 6) & 0x03)];
    image[stride * 3] = alpha[(block_storage[3] >> 1) & 0x07];
    image[stride * 4] = alpha[(block_storage[3] >> 4) & 0x07];
    image[stride * 5] = alpha[((block_storage[4] << 1) & 0x06) | ((block_storage[3] >> 7) & 0x01)];
    image[stride * 6] = alpha[(block_storage[4] >> 2) & 0x07];
    image[stride * 7] = alpha[(block_storage[4] >> 5) & 0x07];
    image[stride * 8] = alpha[block_storage[5] & 0x07];
    image[stride * 9] = alpha[(block_storage[5] >> 3) & 0x07];
    image[stride * 10] = alpha[((block_storage[6] << 2) & 0x04) | ((block_storage[5] >> 6) & 0x03)];
    image[stride * 11] = alpha[(block_storage[6] >> 1) & 0x07];
    image[stride * 12] = alpha[(block_storage[6] >> 4) & 0x07];
    image[stride * 13] = alpha[((block_storage[7] << 1) & 0x06) | ((block_storage[6] >> 7) & 0x01)];
    image[stride * 14] = alpha[(block_storage[7] >> 2) & 0x07];
    image[stride * 15] = alpha[(block_storage[7] >> 5) & 0x07];
}

/**
 * \brief Decompresses one block of a BC2 texture and stores the resulting pixels at the appropriate offset in 'image'.
 *
 * \param block_storage     pointer to the block to decompress.
 * \param image             pointer to image where the decompressed pixel data should be stored.
 **/
void decompress_block_bc2(const uint8_t *block_storage, uint32_t *image) {
    decompress_block_bc1(block_storage + 8, image);

    for (int i = 0; i < 8; i++) {
        image[2 * i] = (((block_storage[i] & 0x0F) | ((block_storage[i] & 0x0F) << 4)) << 24) | (image[2 * i] & 0x00FFFFFF);
        image[2 * i + 1] = (((block_storage[i] & 0xF0) | ((block_storage[i] & 0xF0) >> 4)) << 24) | (image[2 * i + 1] & 0x00FFFFFF);
    }
}

/**
 * \brief Decompresses one block of a BC3 texture and stores the resulting pixels at the appropriate offset in 'image'.
 *
 * \param block_storage     pointer to the block to decompress.
 * \param image             pointer to image where the decompressed pixel data should be stored.
 **/
void decompress_block_bc3(const uint8_t *block_storage, uint32_t *image) {
    decompress_block_bc1(block_storage + 8, image);
    decompress_block_alpha(block_storage, reinterpret_cast<std::uint8_t *>(image), 3, 4);
}

