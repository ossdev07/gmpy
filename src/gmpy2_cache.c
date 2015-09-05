/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * gmpy2_cache.c                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Python interface to the GMP or MPIR, MPFR, and MPC multiple precision   *
 * libraries.                                                              *
 *                                                                         *
 * Copyright 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,               *
 *           2008, 2009 Alex Martelli                                      *
 *                                                                         *
 * Copyright 2008, 2009, 2010, 2011, 2012, 2013, 2014,                     *
 *           2015 Case Van Horsen                                          *
 *                                                                         *
 * This file is part of GMPY2.                                             *
 *                                                                         *
 * GMPY2 is free software: you can redistribute it and/or modify it under  *
 * the terms of the GNU Lesser General Public License as published by the  *
 * Free Software Foundation, either version 3 of the License, or (at your  *
 * option) any later version.                                              *
 *                                                                         *
 * GMPY2 is distributed in the hope that it will be useful, but WITHOUT    *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or   *
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public    *
 * License for more details.                                               *
 *                                                                         *
 * You should have received a copy of the GNU Lesser General Public        *
 * License along with GMPY2; if not, see <http://www.gnu.org/licenses/>    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* gmpy2 caches objects so they can be reused quickly without involving a new
 * memory allocation or object construction.
 *
 * The "py???cache" is used to cache Py??? objects. The cache is accessed
 * via Py???_new/Py???_dealloc. The functions set_py???cache and
 * set_py???cache are used to change the size of the array used to the store
 * the cached objects.
 */

/* Caching logic for Pympz. */

static void
set_gmpympzcache(void)
{
    if (in_gmpympzcache > global.cache_size) {
        int i;
        for (i = global.cache_size; i < in_gmpympzcache; ++i) {
            mpz_clear(gmpympzcache[i]->z);
            PyObject_Del(gmpympzcache[i]);
        }
        in_gmpympzcache = global.cache_size;
    }
    gmpympzcache = GMPY_REALLOC(gmpympzcache, sizeof(MPZ_Object)*global.cache_size);
}

static MPZ_Object *
GMPy_MPZ_New(CTXT_Object *context)
{
    MPZ_Object *result = NULL;

    if (in_gmpympzcache) {
        result = gmpympzcache[--in_gmpympzcache];
        /* Py_INCREF does not set the debugging pointers, so need to use
         * _Py_NewReference instead. */
        _Py_NewReference((PyObject*)result);
        result->hash_cache = -1;
    }
    else {
        if ((result = PyObject_New(MPZ_Object, &MPZ_Type))) {
            mpz_init(result->z);
            result->hash_cache = -1;
        }
    }
    return result;
}

static void
GMPy_MPZ_Dealloc(MPZ_Object *self)
{
    if (in_gmpympzcache < global.cache_size &&
        self->z->_mp_alloc <= global.cache_obsize) {
        gmpympzcache[in_gmpympzcache++] = self;
    }
    else {
        mpz_clear(self->z);
        PyObject_Del(self);
    }
}

/* Caching logic for Pyxmpz. */

static void
set_gmpyxmpzcache(void)
{
    if (in_gmpyxmpzcache > global.cache_size) {
        int i;
        for (i = global.cache_size; i < in_gmpyxmpzcache; ++i) {
            mpz_clear(gmpyxmpzcache[i]->z);
            PyObject_Del(gmpyxmpzcache[i]);
        }
        in_gmpyxmpzcache = global.cache_size;
    }
    gmpyxmpzcache = GMPY_REALLOC(gmpyxmpzcache, sizeof(XMPZ_Object)*global.cache_size);
}

static XMPZ_Object *
GMPy_XMPZ_New(CTXT_Object *context)
{
    XMPZ_Object *result;

    if (in_gmpyxmpzcache) {
        result = gmpyxmpzcache[--in_gmpyxmpzcache];
        /* Py_INCREF does not set the debugging pointers, so need to use
         * _Py_NewReference instead. */
        _Py_NewReference((PyObject*)result);
    }
    else {
        if (!(result = PyObject_New(XMPZ_Object, &XMPZ_Type))) {
            /* LCOV_EXCL_START */
            return NULL;
            /* LCOV_EXCL_STOP */
        }
        mpz_init(result->z);
    }
    return result;
}

static void
GMPy_XMPZ_Dealloc(XMPZ_Object *obj)
{
    if (in_gmpyxmpzcache < global.cache_size &&
        obj->z->_mp_alloc <= global.cache_obsize) {
        gmpyxmpzcache[in_gmpyxmpzcache++] = obj;
    }
    else {
        mpz_clear(obj->z);
        PyObject_Del((PyObject*)obj);
    }
}

/* Caching logic for Pympq. */

static void
set_gmpympqcache(void)
{
    if (in_gmpympqcache > global.cache_size) {
        int i;
        for (i = global.cache_size; i < in_gmpympqcache; ++i) {
            mpq_clear(gmpympqcache[i]->q);
            PyObject_Del(gmpympqcache[i]);
        }
        in_gmpympqcache = global.cache_size;
    }
    gmpympqcache = GMPY_REALLOC(gmpympqcache, sizeof(MPQ_Object)*global.cache_size);
}

static MPQ_Object *
GMPy_MPQ_New(CTXT_Object *context)
{
    MPQ_Object *result;

    if (in_gmpympqcache) {
        result = gmpympqcache[--in_gmpympqcache];
        /* Py_INCREF does not set the debugging pointers, so need to use
           _Py_NewReference instead. */
        _Py_NewReference((PyObject*)result);
    }
    else {
        if (!(result = PyObject_New(MPQ_Object, &MPQ_Type))) {
            /* LCOV_EXCL_START */
            return NULL;
            /* LCOV_EXCL_STOP */
        }
        mpq_init(result->q);
    }
    result->hash_cache = -1;
    return result;
}

static void
GMPy_MPQ_Dealloc(MPQ_Object *self)
{
    if (in_gmpympqcache<global.cache_size &&
        mpq_numref(self->q)->_mp_alloc <= global.cache_obsize &&
        mpq_denref(self->q)->_mp_alloc <= global.cache_obsize) {
        gmpympqcache[in_gmpympqcache++] = self;
    }
    else {
        mpq_clear(self->q);
        PyObject_Del(self);
    }
}

/* Caching logic for Pympfr. */

static void
set_gmpympfrcache(void)
{
    if (in_gmpympfrcache > global.cache_size) {
        int i;
        for (i = global.cache_size; i < in_gmpympfrcache; ++i) {
            mpfr_clear(gmpympfrcache[i]->f);
            PyObject_Del(gmpympfrcache[i]);
        }
        in_gmpympfrcache = global.cache_size;
    }
    gmpympfrcache = GMPY_REALLOC(gmpympfrcache, sizeof(MPFR_Object)*global.cache_size);
}

static MPFR_Object *
GMPy_MPFR_New(mpfr_prec_t bits, CTXT_Object *context)
{
    MPFR_Object *result;

    if (bits < 2) {
        bits = GET_MPFR_PREC(context);
    }

    if (bits < MPFR_PREC_MIN || bits > MPFR_PREC_MAX) {
        VALUE_ERROR("invalid value for precision");
        return NULL;
    }

    if (in_gmpympfrcache) {
        result = gmpympfrcache[--in_gmpympfrcache];
        /* Py_INCREF does not set the debugging pointers, so need to use
           _Py_NewReference instead. */
        _Py_NewReference((PyObject*)result);
        mpfr_set_prec(result->f, bits);
    }
    else {
        if (!(result = PyObject_New(MPFR_Object, &MPFR_Type))) {
            /* LCOV_EXCL_START */
            return NULL;
            /* LCOV_EXCL_STOP */
        }
        mpfr_init2(result->f, bits);
    }
    result->hash_cache = -1;
    result->rc = 0;
    return result;
}

static void
GMPy_MPFR_Dealloc(MPFR_Object *self)
{
    size_t msize;

    /* Calculate the number of limbs in the mantissa. */
    msize = (self->f->_mpfr_prec + mp_bits_per_limb - 1) / mp_bits_per_limb;
    if (in_gmpympfrcache < global.cache_size &&
        msize <= (size_t)global.cache_obsize) {
        gmpympfrcache[in_gmpympfrcache++] = self;
    }
    else {
        mpfr_clear(self->f);
        PyObject_Del(self);
    }
}

static void
set_gmpympccache(void)
{
    if (in_gmpympccache > global.cache_size) {
        int i;
        for (i = global.cache_size; i < in_gmpympccache; ++i) {
            mpc_clear(gmpympccache[i]->c);
            PyObject_Del(gmpympccache[i]);
        }
        in_gmpympccache = global.cache_size;
    }
    gmpympccache = GMPY_REALLOC(gmpympccache, sizeof(MPC_Object)*global.cache_size);
}


static MPC_Object *
GMPy_MPC_New(mpfr_prec_t rprec, mpfr_prec_t iprec, CTXT_Object *context)
{
    MPC_Object *self;

    CHECK_CONTEXT(context);

    if (rprec < 2) {
        rprec = GET_REAL_PREC(context);
    }

    if (iprec < 2) {
        iprec = GET_IMAG_PREC(context);
    }

    if (rprec < MPFR_PREC_MIN || rprec > MPFR_PREC_MAX ||
        iprec < MPFR_PREC_MIN || iprec > MPFR_PREC_MAX) {
        VALUE_ERROR("invalid value for precision");
        return NULL;
    }
    if (in_gmpympccache) {
        self = gmpympccache[--in_gmpympccache];
        /* Py_INCREF does not set the debugging pointers, so need to use
           _Py_NewReference instead. */
        _Py_NewReference((PyObject*)self);
        if (rprec == iprec) {
            mpc_set_prec(self->c, rprec);
        }
        else {
            mpc_clear(self->c);
            mpc_init3(self->c, rprec, iprec);
        }
    }
    else {
        if (!(self = PyObject_New(MPC_Object, &MPC_Type))) {
            /* LCOV_EXCL_START */
            return NULL;
            /* LCOV_EXCL_STOP */
        }
        mpc_init3(self->c, rprec, iprec);
    }
    self->hash_cache = -1;
    self->rc = 0;
    return self;
}

static void
GMPy_MPC_Dealloc(MPC_Object *self)
{
    size_t msize;

    /* Calculate the number of limbs in the mantissa. */
    msize = (mpc_realref(self->c)->_mpfr_prec + mp_bits_per_limb - 1) / mp_bits_per_limb;
    msize += (mpc_imagref(self->c)->_mpfr_prec + mp_bits_per_limb - 1) / mp_bits_per_limb;
    if (in_gmpympccache < global.cache_size &&
        msize <= (size_t)global.cache_obsize) {
        gmpympccache[in_gmpympccache++] = self;
    }
    else {
        mpc_clear(self->c);
        PyObject_Del(self);
    }
}

