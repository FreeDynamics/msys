#include "spatial_hash.hxx"
#include <limits>
#include <stdio.h>

#define EXCLUDE_SELF_CONTACTS

namespace desres { namespace msys {

// do only bounding box checks on query atoms, no spatial hashing.
template<>
IdList SpatialHashT<float>::find_within_small(float r, const float* pos,
    int n, const Id* ids) const {

    IdList result;
    const float r2 = r*r;
    float xlo = xmin - r;
    float ylo = ymin - r;
    float zlo = zmin - r;

    float xhi = xmax + r;
    float yhi = ymax + r;
    float zhi = zmax + r;

    for (int i=0; i<n; i++) {
        Id id = ids[i];
        const float* p = pos + 3*id;
        const float x = p[0];
        const float y = p[1];
        const float z = p[2];

        if (x<xlo || y<ylo || z<zlo ||
            x>xhi || y>yhi || z>zhi) {
            continue;
        }

        for (int j=0; j<ntarget; j++) {
            float dx = x - _x[j];
            float dy = y - _y[j];
            float dz = z - _z[j];
            float d2 = dx*dx + dy*dy + dz*dz;
            if (d2 <= r2) {
                result.push_back(id);
                break;
            }
        }
    }
    return result;
}

template<>
bool SpatialHashT<float>::test2(float r2, int voxid, float x, float y, float z) const {
    for (int i=0; i<10; i++) {
        int vox = voxid + full_shell[i];
        uint32_t b = _counts[vox], e = _counts[vox+strip_lens[i]];
        const float * xi = _x+b;
        const float * yi = _y+b;
        const float * zi = _z+b;

        for (; b<e; ++b, ++xi, ++yi, ++zi) {
            float dx = x - *xi;
            float dy = y - *yi;
            float dz = z - *zi;
            float d2 = dx*dx + dy*dy + dz*dz;
            if (d2<=r2) return true;
        }
    }
    return false;
}

template<>
IdList SpatialHashT<float>::find_within(float r, const float* pos, 
                  int n, const Id* ids) const {

    bool periodic = cx!=0 || cy!=0 || cz!=0;

    // pbwithin not implemented for find_within_small.
    // threshold based on minimal testing.
    if (!periodic && ntarget < 800) return find_within_small(r,pos,n,ids);

    IdList result;
    result.reserve(n);
    int j=0;

    float tmp[12];

    for (; j<n; j++) {
        unsigned id = ids[j];
        const float *xyz = pos + 3*id;
        if (rot) {
            memcpy(tmp,xyz,3*sizeof(float));
            pfx::apply_rotation(1,tmp,rot);
            xyz = tmp;
        }
        float x=xyz[0], y=xyz[1], z=xyz[2];
        if (test(r, x,y,z) || (periodic && minimage(r,cx,cy,cz,x,y,z))) {
            result.push_back(id);
        }
    }
    return result;
}

template<>
void SpatialHashT<float>::find_contacts(float r2, int voxid, float x, float y, float z,
                           Id id, contact_array_t* result) const {

    result->reserve_additional(27*maxcount);
    Id* ri = result->i;
    Id* rj = result->j;
    float* rd = result->d2;
    uint64_t count = result->count;

    for (int i=0; i<10; i++) {
        int vox = voxid + full_shell[i];
        uint32_t b = _counts[vox], e = _counts[vox+strip_lens[i]];
        const float * xi = _x+b;
        const float * yi = _y+b;
        const float * zi = _z+b;
        const Id    * ii = _ids+b;

        /* stragglers */
        for (; b<e; ++b, ++xi, ++yi, ++zi, ++ii) {
#ifdef EXCLUDE_SELF_CONTACTS
            if (id==*ii) continue;
#endif
            float dx = x - *xi;
            float dy = y - *yi;
            float dz = z - *zi;
            float d2 = dx*dx + dy*dy + dz*dz;
            if (d2<=r2) {
                ri[count] = id;
                rj[count] = *ii;
                rd[count] = d2;
                ++count;
            }
        }
    }
    result->count = count;
}

}}

