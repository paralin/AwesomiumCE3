#ifndef _COMPTR_H
#define _COMPTR_H

//! A template COM smart pointer.
template<typename I>
class ComPtr
{
    public:
        //! Constructs an empty smart pointer.
        ComPtr() : m_p( 0 ) {}

        //! Assumes ownership of the given instance, if non-null.
        /*! \param  p   A pointer to an existing COM instance, or 0 to create an
                        empty COM smart pointer.
            \note The smart pointer will assume ownership of the given instance.
            It will \b not AddRef the contents, but it will Release the object
            as it goes out of scope.
        */
        explicit ComPtr( I* p ) : m_p( p ) {}

        //! Releases the contained instance.
        ~ComPtr()
        {
            SafeRelease();
        }

        //! Copy-construction.
        ComPtr( ComPtr<I> const& ptr ) : m_p( ptr.m_p )
        {
            if ( m_p )
            {
                m_p->AddRef();
            }
        }

        //! Assignment.
        ComPtr<I>& operator=( ComPtr<I> const& ptr )
        {
            ComPtr<I> copy( ptr );
            Swap( copy );
            return *this;
        }

        //! Releases a contained instance, if present.
        /*! \note You should never need to call this function unless you wish to
            take control a Release an instance before the smart pointer goes out
            of scope.
        */
        void SafeRelease()
        {
            if ( m_p )
            {
                m_p->Release();
            }

            m_p = 0;
        }

        //! Explicitly gets the address of the pointer.
        /*! \note This function should not be called on a smart pointer with
            non-zero contents. This is to avoid memory leaks by blatting over the
            contents without calling Release. Hence the complaint to std::cerr.
        */
        I** AddressOf()
        {
            if ( m_p )
            {
                std::cerr << __FUNCTION__
                          << ": non-zero contents - possible memory leak" << std::endl;
            }

            return &m_p;
        }

        //! Gets the encapsulated pointer.
        I* Get() const
        {
            return m_p;
        }

        //! Gets the encapsulated pointer.
        I* operator->() const
        {
            return m_p;
        }

        //! Swaps the encapsulated pointer with that of the argument.
        void Swap( ComPtr<I>& ptr )
        {
            I* p = m_p;
            m_p = ptr.m_p;
            ptr.m_p = p;
        }

        //! Returns true if non-empty.
        operator bool() const
        {
            return m_p != 0;
        }

    private:
        //! The encapsulated instance.
        I* m_p;
};

#endif // ndef _COMPTR_H