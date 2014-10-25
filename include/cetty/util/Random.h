#if !defined(CETTY_UTIL_RANDOM_H)
#define CETTY_UTIL_RANDOM_H

/*
 * Copyright (c) 1995, 2007, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <time.h>
#include <xutility>
#include <boost/integer.hpp>

namespace cetty {
namespace util {

/**
 * An instance of this class is used to generate a stream of
 * pseudorandom numbers. The class uses a 48-bit seed, which is
 * modified using a linear congruential formula. (See Donald Knuth,
 * <i>The Art of Computer Programming, Volume 3</i>, Section 3.2.1.)
 * <p>
 * If two instances of <tt>Random</tt> are created with the same
 * seed, and the same sequence of method calls is made for each, they
 * will generate and return identical sequences of numbers. In order to
 * guarantee this property, particular algorithms are specified for the
 * class <tt>Random</tt>. Java implementations must use all the algorithms
 * shown here for the class <tt>Random</tt>, for the sake of absolute
 * portability of Java code. However, subclasses of class <tt>Random</tt>
 * are permitted to use other algorithms, so long as they adhere to the
 * general contracts for all the methods.
 * <p>
 * The algorithms implemented by class <tt>Random</tt> use a
 * <tt>protected</tt> utility method that on each invocation can supply
 * up to 32 pseudorandomly generated bits.
 * <p>
 * Many applications will find the method {@link Math#random} simpler to use.
 *
 * @author  Frank Yellin
 * @since   1.0
 */

class Random {
public:
    /**
     * Creates a new random number generator. This constructor sets
     * the seed of the random number generator to a value very likely
     * to be distinct from any other invocation of this constructor.
     */
    Random() {
        static int64_t seedUniquifier = 8682522807148012L;
        time_t t;
        /*this(++seedUniquifier + System.nanoTime());*/
        setSeed(++seedUniquifier + time(&t));
    }

    /**
     * Creates a new random number generator using a single <tt>long</tt> seed.
     * The seed is the initial value of the internal state of the pseudorandom
     * number generator which is maintained by method {@link #next}.
     *
     * <p>The invocation <tt>new Random(seed)</tt> is equivalent to:
     *  <pre> @code
     * Random rnd = new Random();
     * rnd.setSeed(seed);@endcode</pre>
     *
     * @param seed the initial seed
     * @see   #setSeed(long)
     */
    Random(int64_t seed) {
        //this.seed = new AtomicLong(0L);
        setSeed(seed);
    }

    /**
     * Sets the seed of this random number generator using a single
     * <tt>long</tt> seed. The general contract of <tt>setSeed</tt> is
     * that it alters the state of this random number generator object
     * so as to be in exactly the same state as if it had just been
     * created with the argument <tt>seed</tt> as a seed. The method
     * <tt>setSeed</tt> is implemented by class <tt>Random</tt> by
     * atomically updating the seed to
     *  <pre><tt>(seed ^ 0x5DEECE66DL) & ((1L << 48) - 1)</tt></pre>
     * and clearing the <tt>haveNextNextGaussian</tt> flag used by {@link
     * #nextGaussian}.
     *
     * <p>The implementation of <tt>setSeed</tt> by class <tt>Random</tt>
     * happens to use only 48 bits of the given seed. In general, however,
     * an overriding method may use all 64 bits of the <tt>long</tt>
     * argument as a seed value.
     *
     * @param seed the initial seed
     */
    void setSeed(int64_t seed) {
        // TODO: need lock here.

        seed = (seed ^ multiplier) & mask;
        this->seed = seed;
        //this->seed.set(seed);
    }

    /**
     * Generates random bytes and places them into a user-supplied
     * byte array.  The number of random bytes produced is equal to
     * the length of the byte array.
     *
     * <p>The method <tt>nextBytes</tt> is implemented by class <tt>Random</tt>
     * as if by:
     *  <pre> @code
     * public void nextBytes(byte[] bytes) {
     *   for (int i = 0; i < bytes.length; )
     *     for (int rnd = nextInt(), n = Math.min(bytes.length - i, 4);
     *          n-- > 0; rnd >>= 8)
     *       bytes[i++] = (byte)rnd;
     * }@endcode</pre>
     *
     * @param  bytes the byte array to fill with random bytes
     * @throws NullPointerException if the byte array is null
     * @since  1.1
     */
    void nextBytes(char* bytes, int size) {
        if (NULL == bytes || size <= 0) { return; }

        for (int i = 0, len = size; i < len;) {
#ifdef _WIN32
            for (int rnd = nextInt(), n = min(len - i, (int)(sizeof(int)/sizeof(char)));
#else
            for (int rnd = nextInt(), n = std::min(len - i, (int)(sizeof(int)/sizeof(char)));
#endif
                    n-- > 0; rnd >>= sizeof(char)) {
                bytes[i++] = (char)rnd;
            }
        }
    }

    /**
     * Returns the next pseudorandom, uniformly distributed <tt>int</tt>
     * value from this random number generator's sequence. The general
     * contract of <tt>nextInt</tt> is that one <tt>int</tt> value is
     * pseudorandomly generated and returned. All 2<font size="-1"><sup>32
     * </sup></font> possible <tt>int</tt> values are produced with
     * (approximately) equal probability.
     *
     * <p>The method <tt>nextInt</tt> is implemented by class <tt>Random</tt>
     * as if by:
     *  <pre> @code
     * public int nextInt() {
     *   return next(32);
     * }@endcode</pre>
     *
     * @return the next pseudorandom, uniformly distributed <tt>int</tt>
     *         value from this random number generator's sequence
     */
    int nextInt() {
        return next(32);
    }

    /**
     * Returns a pseudorandom, uniformly distributed <tt>int</tt> value
     * between 0 (inclusive) and the specified value (exclusive), drawn from
     * this random number generator's sequence.  The general contract of
     * <tt>nextInt</tt> is that one <tt>int</tt> value in the specified range
     * is pseudorandomly generated and returned.  All <tt>n</tt> possible
     * <tt>int</tt> values are produced with (approximately) equal
     * probability.  The method <tt>nextInt(int n)</tt> is implemented by
     * class <tt>Random</tt> as if by:
     *  <pre> @code
     * public int nextInt(int n) {
     *   if (n <= 0)
     *     throw new IllegalArgumentException("n must be positive");
     *
     *   if ((n & -n) == n)  // i.e., n is a power of 2
     *     return (int)((n * (long)next(31)) >> 31);
     *
     *   int bits, val;
     *   do {
     *       bits = next(31);
     *       val = bits % n;
     *   } while (bits - val + (n-1) < 0);
     *   return val;
     * }@endcode</pre>
     *
     * <p>The hedge "approximately" is used in the foregoing description only
     * because the next method is only approximately an unbiased source of
     * independently chosen bits.  If it were a perfect source of randomly
     * chosen bits, then the algorithm shown would choose <tt>int</tt>
     * values from the stated range with perfect uniformity.
     * <p>
     * The algorithm is slightly tricky.  It rejects values that would result
     * in an uneven distribution (due to the fact that 2^31 is not divisible
     * by n). The probability of a value being rejected depends on n.  The
     * worst case is n=2^30+1, for which the probability of a reject is 1/2,
     * and the expected number of iterations before the loop terminates is 2.
     * <p>
     * The algorithm treats the case where n is a power of two specially: it
     * returns the correct number of high-order bits from the underlying
     * pseudo-random number generator.  In the absence of special treatment,
     * the correct number of <i>low-order</i> bits would be returned.  Linear
     * congruential pseudo-random number generators such as the one
     * implemented by this class are known to have short periods in the
     * sequence of values of their low-order bits.  Thus, this special case
     * greatly increases the length of the sequence of values returned by
     * successive calls to this method if n is a small power of two.
     *
     * @param n the bound on the random number to be returned.  Must be
     *        positive.
     * @return the next pseudorandom, uniformly distributed <tt>int</tt>
     *         value between <tt>0</tt> (inclusive) and <tt>n</tt> (exclusive)
     *         from this random number generator's sequence
     * @exception IllegalArgumentException if n is not positive
     * @since 1.2
     */

    int nextInt(int n) {
        if (n <= 0) {
            throw std::invalid_argument("n must be positive");
        }

        //return rand() % n;

        if ((n & -n) == n) {// i.e., n is a power of 2
            return (int)((n * ((int64_t)next(31))) >> 31);
        }

        int bits, val;

        do {
            bits = next(31);
            val = bits % n;
        }
        while (bits - val + (n-1) < 0);

        return val;
    }

    /**
     * Returns the next pseudorandom, uniformly distributed <tt>long</tt>
     * value from this random number generator's sequence. The general
     * contract of <tt>nextLong</tt> is that one <tt>long</tt> value is
     * pseudorandomly generated and returned.
     *
     * <p>The method <tt>nextLong</tt> is implemented by class <tt>Random</tt>
     * as if by:
     *  <pre> @code
     * public long nextLong() {
     *   return ((long)next(32) << 32) + next(32);
     * }@endcode</pre>
     *
     * Because class <tt>Random</tt> uses a seed with only 48 bits,
     * this algorithm will not return all possible <tt>long</tt> values.
     *
     * @return the next pseudorandom, uniformly distributed <tt>long</tt>
     *         value from this random number generator's sequence
     */
    int64_t nextLong() {
        // it's okay that the bottom word remains signed.
        return ((int64_t)(next(32)) << 32) + next(32);
    }

    /**
     * Returns the next pseudorandom, uniformly distributed
     * <tt>boolean</tt> value from this random number generator's
     * sequence. The general contract of <tt>nextBoolean</tt> is that one
     * <tt>boolean</tt> value is pseudorandomly generated and returned.  The
     * values <tt>true</tt> and <tt>false</tt> are produced with
     * (approximately) equal probability.
     *
     * <p>The method <tt>nextBoolean</tt> is implemented by class <tt>Random</tt>
     * as if by:
     *  <pre> @code
     * public boolean nextBoolean() {
     *   return next(1) != 0;
     * }@endcode</pre>
     *
     * @return the next pseudorandom, uniformly distributed
     *         <tt>boolean</tt> value from this random number generator's
     *         sequence
     * @since 1.2
     */
    bool nextBool() {
        return next(1) != 0;
    }

    /**
     * Returns the next pseudorandom, uniformly distributed <tt>float</tt>
     * value between <tt>0.0</tt> and <tt>1.0</tt> from this random
     * number generator's sequence.
     *
     * <p>The general contract of <tt>nextFloat</tt> is that one
     * <tt>float</tt> value, chosen (approximately) uniformly from the
     * range <tt>0.0f</tt> (inclusive) to <tt>1.0f</tt> (exclusive), is
     * pseudorandomly generated and returned. All 2<font
     * size="-1"><sup>24</sup></font> possible <tt>float</tt> values
     * of the form <i>m&nbsp;x&nbsp</i>2<font
     * size="-1"><sup>-24</sup></font>, where <i>m</i> is a positive
     * integer less than 2<font size="-1"><sup>24</sup> </font>, are
     * produced with (approximately) equal probability.
     *
     * <p>The method <tt>nextFloat</tt> is implemented by class <tt>Random</tt>
     * as if by:
     *  <pre> @code
     * public float nextFloat() {
     *   return next(24) / ((float)(1 << 24));
     * }@endcode</pre>
     *
     * <p>The hedge "approximately" is used in the foregoing description only
     * because the next method is only approximately an unbiased source of
     * independently chosen bits. If it were a perfect source of randomly
     * chosen bits, then the algorithm shown would choose <tt>float</tt>
     * values from the stated range with perfect uniformity.<p>
     * [In early versions of Java, the result was incorrectly calculated as:
     *  <pre> @code
     *   return next(30) / ((float)(1 << 30));@endcode</pre>
     * This might seem to be equivalent, if not better, but in fact it
     * introduced a slight nonuniformity because of the bias in the rounding
     * of floating-point numbers: it was slightly more likely that the
     * low-order bit of the significand would be 0 than that it would be 1.]
     *
     * @return the next pseudorandom, uniformly distributed <tt>float</tt>
     *         value between <tt>0.0</tt> and <tt>1.0</tt> from this
     *         random number generator's sequence
     */
    float nextFloat() {
        return next(24) / ((float)(1 << 24));
    }

    /**
     * Returns the next pseudorandom, uniformly distributed
     * <tt>double</tt> value between <tt>0.0</tt> and
     * <tt>1.0</tt> from this random number generator's sequence.
     *
     * <p>The general contract of <tt>nextDouble</tt> is that one
     * <tt>double</tt> value, chosen (approximately) uniformly from the
     * range <tt>0.0d</tt> (inclusive) to <tt>1.0d</tt> (exclusive), is
     * pseudorandomly generated and returned.
     *
     * <p>The method <tt>nextDouble</tt> is implemented by class <tt>Random</tt>
     * as if by:
     *  <pre> @code
     * public double nextDouble() {
     *   return (((long)next(26) << 27) + next(27))
     *     / (double)(1L << 53);
     * }@endcode</pre>
     *
     * <p>The hedge "approximately" is used in the foregoing description only
     * because the <tt>next</tt> method is only approximately an unbiased
     * source of independently chosen bits. If it were a perfect source of
     * randomly chosen bits, then the algorithm shown would choose
     * <tt>double</tt> values from the stated range with perfect uniformity.
     * <p>[In early versions of Java, the result was incorrectly calculated as:
     *  <pre> @code
     *   return (((long)next(27) << 27) + next(27))
     *     / (double)(1L << 54);@endcode</pre>
     * This might seem to be equivalent, if not better, but in fact it
     * introduced a large nonuniformity because of the bias in the rounding
     * of floating-point numbers: it was three times as likely that the
     * low-order bit of the significand would be 0 than that it would be 1!
     * This nonuniformity probably doesn't matter much in practice, but we
     * strive for perfection.]
     *
     * @return the next pseudorandom, uniformly distributed <tt>double</tt>
     *         value between <tt>0.0</tt> and <tt>1.0</tt> from this
     *         random number generator's sequence
     * @see Math#random
     */
    double nextDouble() {
        return (((int64_t)(next(26)) << 27) + next(27)) / (double)(0x0020000000000000); // 1 << 53
    }

protected:
    /**
     * Generates the next pseudorandom number. Subclasses should
     * override this, as this is used by all other methods.
     *
     * <p>The general contract of <tt>next</tt> is that it returns an
     * <tt>int</tt> value and if the argument <tt>bits</tt> is between
     * <tt>1</tt> and <tt>32</tt> (inclusive), then that many low-order
     * bits of the returned value will be (approximately) independently
     * chosen bit values, each of which is (approximately) equally
     * likely to be <tt>0</tt> or <tt>1</tt>. The method <tt>next</tt> is
     * implemented by class <tt>Random</tt> by atomically updating the seed to
     *  <pre><tt>(seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1)</tt></pre>
     * and returning
     *  <pre><tt>(int)(seed >>> (48 - bits))</tt>.</pre>
     *
     * This is a linear congruential pseudorandom number generator, as
     * defined by D. H. Lehmer and described by Donald E. Knuth in
     * <i>The Art of Computer Programming,</i> Volume 3:
     * <i>Seminumerical Algorithms</i>, section 3.2.1.
     *
     * @param  bits random bits
     * @return the next pseudorandom value from this random number
     *         generator's sequence
     * @since  1.1
     */
    int next(int bits) {
        int64_t oldseed, nextseed;

        //int64_t/*AtomicLong*/ seed = this->seed;
        do {
            oldseed = seed;
            nextseed = (oldseed * multiplier + addend) & mask;

            //
            if (oldseed != nextseed) {
                this->seed = nextseed;
                break;
            }
        }
        while (/*!seed.compareAndSet(oldseed, nextseed)*/true);

        return (int)(nextseed >> (48 - bits));
    }

private:
    static const int64_t multiplier = 0x5DEECE66D;
    static const int64_t addend = 0xB;
    static const int64_t mask = 0x0000FFFFFFFFFFFF;

    //static int64_t seedUniquifier;//;

    /**
     * The internal state associated with this pseudorandom number generator.
     * (The specs for the methods in this class describe the ongoing
     * computation of this value.)
     */
    int64_t/*AtomicLong*/ seed;
};

}
}

#endif //#if !defined(CETTY_UTIL_RANDOM_H)

// Local Variables:
// mode: c++
// End:
