#include "TarLoader.h"
extern "C" {
#include "string.h"
}

TarLoader::TarLoader(AssetLoader *loader) : m_loader(loader) {
    m_reader = bufferreader_create();
}

TarLoader::~TarLoader() {
    bufferreader_destroy(m_reader);
}

static int tar_read(mtar_t *tar, void *data, unsigned size) {
    BufferReader *reader = static_cast<BufferReader *>(tar->stream);
    if (tar->pos + size >= bufferreader_data_length(reader)) {
        return MTAR_EREADFAIL;
    }
    memcpy(data, bufferreader_data(reader) + tar->pos, size);
    return MTAR_ESUCCESS;
}

static int tar_seek(mtar_t *tar, unsigned pos) {
    tar->pos = pos;
    return MTAR_ESUCCESS;
}

static int tar_close(mtar_t *) {
    return MTAR_ESUCCESS;
}


void TarLoader::load(BufferReader *reader, const std::string &filename) {
    mtar_t tar;
    mtar_header_t h;

    memset(&tar, 0, sizeof(tar));
    tar.stream = reader;
    tar.read = &tar_read;
    tar.seek = &tar_seek;
    tar.close = &tar_close;

    while ((mtar_read_header(&tar, &h)) == MTAR_ESUCCESS) {
        if (m_loader->willLoad(h.name)) {
            bufferreader_init_from_tar_file(m_reader, &tar, &h);
            m_loader->load(m_reader, filename + ":/" + h.name);
        }
        mtar_next(&tar);
    }

    mtar_close(&tar);
}
