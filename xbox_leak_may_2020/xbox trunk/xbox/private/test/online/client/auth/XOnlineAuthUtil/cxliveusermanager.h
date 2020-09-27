
#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xonlinep.h>
#include <algorithm>
#include <functional>
#include <vector>
#include <exception>
#include <Assert.h>
#include <new>

#pragma once



namespace XLive
{


//
/////////////////////// Iterator base class /////////////////////////
//
class UserAccountIterator : public std::bidirectional_iterator_tag
{
    friend class CXLiveUserManager;
public:
    inline UserAccountIterator(UserAccountIterator const &rhs)
        : m_pUserMgr(rhs.m_pUserMgr), m_pUser(rhs.m_pUser)
    {
    }

    // Primary constructor
    inline explicit UserAccountIterator(CXLiveUserManager const * const pUserMgr, XONLINE_USER *pUser)
        :   m_pUserMgr(pUserMgr),
            m_pUser(pUser)
    {
    }

    inline explicit UserAccountIterator()     // For STL
        : m_pUserMgr(NULL), m_pUser(NULL)
    {
    }

    virtual inline ~UserAccountIterator() { ; }

    virtual inline UserAccountIterator& operator=(UserAccountIterator const &rhs)
    {
        new (this) UserAccountIterator(rhs);
        return *this;
    }

    inline XONLINE_USER& operator*() const  { return *m_pUser; }
    inline XONLINE_USER* operator->() const { return  m_pUser; }

    // Relational operators
    inline bool operator==(UserAccountIterator const &rhs) const  { return rhs.m_pUser == m_pUser; }
    inline bool operator!=(UserAccountIterator const &rhs) const  { return !(rhs == *this); }
    inline bool operator<(UserAccountIterator const &rhs)  const  { return m_pUser < rhs.m_pUser; }
    inline bool operator<=(UserAccountIterator const &rhs)  const  { return m_pUser <= rhs.m_pUser; }
    inline bool operator>(UserAccountIterator const &rhs)  const  { return m_pUser > rhs.m_pUser; }
    inline bool operator>=(UserAccountIterator const &rhs)  const  { return m_pUser >= rhs.m_pUser; }

protected:
    // Member variables.
    CXLiveUserManager const * const m_pUserMgr;
    XONLINE_USER                    *m_pUser;
};


//
////////////////////////////////////////////////////////////////
//

/////////////////////// Useful function objects ////////////////
//
class IsAccount:
    public std::unary_function< XONLINE_USER const&, bool >
{
public:
    inline bool operator()(XONLINE_USER const& user) const
    {
        return (user.xuid.qwUserID != 0);
    }
};

class IsMUAccount:
    public std::unary_function< XONLINE_USER const&, bool >
{
public:
    inline bool operator()(XONLINE_USER const& user) const
    {
        return ((user.dwUserOptions & XONLINE_USER_OPTION_CAME_FROM_MU) &&
            (user.xuid.qwUserID != 0));
    }
};

class IsHDAccount:
    public std::unary_function< XONLINE_USER const&, bool >
{
public:
    inline bool operator()(XONLINE_USER const& user) const
    {
        return (!(user.dwUserOptions & XONLINE_USER_OPTION_CAME_FROM_MU))
                    && (user.xuid.qwUserID != 0);
    }
};

class IsGuest:
    public std::unary_function< XONLINE_USER const&, bool >
{
public:
    inline bool operator()(XONLINE_USER const& user) const
    {
        return ((user.dwUserOptions & XONLINE_USER_GUEST_MASK) &&
            (user.xuid.qwUserID != 0));
    }
};

class UserAccountHasName:
    public std::unary_function< XONLINE_USER const&, bool >
{
public:
    inline bool operator()(XONLINE_USER const& user) const
    {
        return (user.name[0] != static_cast<CHAR>('\0'));
    }
};

class UserAccountHasXUID:
    public std::unary_function< XONLINE_USER const&, bool >
{
public:
    inline bool operator()(XONLINE_USER const& user) const
    {
        return (user.xuid.qwUserID != 0);
    }
};


//
////////////////////////////////////////////////////////////////////
//

class CXLiveUserManager
{
public:

    // Container type declarations
    typedef XONLINE_USER value_type;
    typedef size_t size_type;
    typedef size_t difference_type;
    typedef XONLINE_USER& reference;
    typedef XONLINE_USER const & const_reference;

    // Forward declarations
    class MUAccountIterator;
    class HDAccountIterator;
    class AccountIterator;
    class const_AccountIterator;

public:

    explicit CXLiveUserManager()
        :   m_pStart(&(m_users[0])),
            m_pEnd((&m_users[0]) + XONLINE_MAX_STORED_ONLINE_USERS - 1)
    {
        ZeroMemory(&(m_users[0]), sizeof(m_users));
        ZeroMemory(&m_selectedAccounts[0], sizeof(m_selectedAccounts));
        ComputeAccounts();
        ComputeMUAccounts();
        ComputeHDAccounts();
    }

    explicit CXLiveUserManager(XONLINE_USER const *users)
        :   m_pStart(m_users),
            m_pEnd((m_users + XONLINE_MAX_STORED_ONLINE_USERS - 1)),
            m_numberOfSelectedAccounts(0)
    {
        memcpy(m_users, users, sizeof(m_users));
        ZeroMemory(m_selectedAccounts, sizeof(m_selectedAccounts));
        ComputeAccounts();
        ComputeMUAccounts();
        ComputeHDAccounts();
    }

    ~CXLiveUserManager();

    //
    // Class operators ///////////////////////////////////////////////
    //

    bool operator==(CXLiveUserManager const &rhs) const
    {
        for (size_t i = 0; i < XONLINE_MAX_STORED_ONLINE_USERS; ++i)
        {
            if (memcmp(m_users + i, rhs.m_users + i, sizeof(XONLINE_USER)) != 0)
            {
                return false;
            }
        }
        return true;
    }

    inline bool operator!=(CXLiveUserManager const &rhs) const  { return !(*this == rhs); }

    inline XONLINE_USER& operator[]( int element ) const
    {
//       Assert(element >= 0 && element < XONLINE_MAX_STORED_ONLINE_USERS);
        return *(m_pStart + element);
    }

    //
    // Standard container support ////////////////////////////////////////
    //

    inline XONLINE_USER &front()                { return *m_pStart; }
    inline XONLINE_USER &back()                 { return *m_pEnd; }
    inline XONLINE_USER const& front() const    { return *m_pStart; }
    inline XONLINE_USER const& back()  const    { return *m_pEnd; }

    inline XONLINE_USER& at(size_type pos)
    {
//        Assert(pos >= 0 && pos < XONLINE_MAX_STORED_ONLINE_USERS);
        if (pos < 0 || pos >= XONLINE_MAX_STORED_ONLINE_USERS)
        {
            throw std::out_of_range("CXLiveUserManager: Invalid subscript!");
        }

        return *(m_pStart + pos);
    }

    inline XONLINE_USER const & at(size_type pos) const
    {
//        Assert(pos >= 0 && pos < XONLINE_MAX_STORED_ONLINE_USERS);
        if (pos < 0 || pos >= XONLINE_MAX_STORED_ONLINE_USERS)
        {
            throw std::out_of_range("CXLiveUserManager: Invalid subscript!");
        }

        return *(m_pStart + pos);
    }

    inline size_t size() const  { return std::count_if(m_pStart, m_pEnd, UserAccountHasXUID()); }
    inline bool empty() const   { return std::find_if( m_pStart, m_pEnd, UserAccountHasXUID()) == m_pEnd; }
    inline size_t max_size() const  { return m_pEnd - m_pStart; }
    inline size_t capacity() const  { return m_pEnd - m_pStart; }

    AccountIterator erase(AccountIterator it)
    {
//        Assert(it >= begin());
//        Assert(it <= end());

        if (it.m_pUser == m_pEnd)
        {
            return it;
        }

        ZeroMemory(it.m_pUser, sizeof(XONLINE_USER));

        // See if we need to recompute these pointers.
        if (IsMUAccount(it))
        {
            ComputeAccounts();
        }

        if (IsMUAccount(it))
        {
            ComputeMUAccounts();
        }

        if (IsHDAccount(it))
        {
            ComputeHDAccounts();
        }

        return AccountIterator(this, it.m_pUser + 1);
    }

    AccountIterator erase(AccountIterator first, AccountIterator last)
    {
//        Assert(last >= first);
//        Assert(last <= end());
//        Assert(first >= begin());

        size_t count = last.m_pUser - first.m_pUser;
        ZeroMemory(first.m_pUser, sizeof(XONLINE_USER) * count);

        ComputeAccounts();
        return (last == Account_end()) ? last : AccountIterator(this, last.m_pUser + 1);
    }

    MUAccountIterator erase(MUAccountIterator first, MUAccountIterator last)
    {
//        Assert(last >= first);
//        Assert(last <= end());
//        Assert(first >= begin());

        for (; first != last; ++first)
        {
            (void) erase(first);
        }

        ComputeMUAccounts();
        return (last == MUAccount_end()) ? last : ++last;
    }

    MUAccountIterator erase(MUAccountIterator it)
    {
        if (it == MUAccount_end())
        {
            return it;
        }

        ZeroMemory(it.m_pUser, sizeof(XONLINE_USER));
        ComputeMUAccounts();
        return ++it;
    }

    HDAccountIterator erase(HDAccountIterator it)
    {
        if (it == HDAccount_end())
        {
            return it;
        }

        ZeroMemory(it.m_pUser, sizeof(XONLINE_USER));
        ComputeHDAccounts();
        return ++it;
    }

    HDAccountIterator erase(HDAccountIterator first, HDAccountIterator last)
    {
//        Assert(last >= first);
//        Assert(last <= end());
//        Assert(first >= begin());

        for (; first != last; ++first)
        {
            (void) erase(first);
        }

        ComputeHDAccounts();
        return (last == HDAccount_end()) ? last : ++last;
    }

    inline void clear()
    {
        (void) erase(Account_begin(), Account_end());
        ComputeAccounts();
        ComputeMUAccounts();
        ComputeHDAccounts();
    }

    //
    // The following operator exposes the internal table for use
    // by XOnlineLogon() and other APIs.
    inline operator XONLINE_USER*() { return m_pStart; }

    bool VerifyNoDuplicates() const;

    HRESULT ClearAccountSelections()
    {
        ZeroMemory(&m_selectedAccounts[0], sizeof(m_selectedAccounts));
        m_numberOfSelectedAccounts = 0;
        return S_OK;
    }

    HRESULT SelectAccount(UserAccountIterator it)
    {
        if (m_numberOfSelectedAccounts < XONLINE_MAX_LOGON_USERS)
        {
            m_selectedAccounts[ m_numberOfSelectedAccounts++ ] = *it;
            return S_OK;
        }
        else
        {
            return E_INVALIDARG;
        }
    }

    inline XONLINE_USER *SelectedAccounts() { return &m_selectedAccounts[0]; }

    static HRESULT PopulateMUs(DWORD *pCount);

    HRESULT GenerateAndSelectRandomAccounts(    int hdMasterAccounts,
                                                int hdGuestAccounts,
                                                int muMasterAccounts,
                                                int muGuestAccounts);

    HRESULT GenerateAndSelectRandomAccounts()
    {
        return GenerateAndSelectRandomAccounts(1, 1, 1, 1);
    }

    bool UserIsSelected(const XUID puid) const
    {
        for (   const XONLINE_USER *pUser = m_selectedAccounts;
                pUser < m_selectedAccounts + XONLINE_MAX_LOGON_USERS;
                ++pUser)
        {
            if (pUser->xuid == puid)
            {
                return true;
            }
        }
        return false; 
    }


//
//////////////////////// Define iterator support
//

    inline static bool IsAccount(XONLINE_USER const *pUser)
    {
        XLive::IsAccount muTest;
        return muTest(*pUser);
    }

    inline static bool IsGuestAccount(XONLINE_USER const *pUser)
    {
        XLive::IsGuest muTest;
        return muTest(*pUser);
    }

    inline static bool IsMUAccount(XONLINE_USER const *pUser)
    {
        XLive::IsMUAccount muTest;
        return muTest(*pUser);
    }

    inline static bool IsHDAccount(XONLINE_USER const *pUser)
    {
        XLive::IsHDAccount muTest;
        return muTest(*pUser);
    }

    inline AccountIterator Account_begin()
    {
        if (m_pFirstAccount == NULL)
        {
            return AccountIterator(this, m_pEnd);
        }
        return AccountIterator(this, m_pFirstAccount);
    }

    inline AccountIterator Account_end()
    {
        if (m_pFirstAccount == NULL)
        {
            return AccountIterator(this, m_pEnd);
        }
        return AccountIterator(this, m_pLastAccount + 1);
    }

    inline const_AccountIterator Account_begin() const
    {
        if (m_pFirstAccount == NULL)
        {
            return const_AccountIterator(this, m_pEnd);
        }
        return const_AccountIterator(this, m_pFirstAccount);
    }

    inline const_AccountIterator Account_end() const
    {
        if (m_pFirstAccount == NULL)
        {
            return const_AccountIterator(this, m_pEnd);
        }
        return const_AccountIterator(this, m_pLastAccount + 1);
    }

    inline MUAccountIterator MUAccount_begin() const
    {
        if (m_pFirstMUAccount == NULL)
        {
            return MUAccountIterator(this, m_pEnd);
        }
        return MUAccountIterator(this, m_pFirstMUAccount);
    }

    inline MUAccountIterator MUAccount_end() const
    {
        if (m_pFirstMUAccount == NULL)
        {
            return MUAccountIterator(this, m_pEnd);
        }
        return MUAccountIterator(this, m_pLastMUAccount + 1);
    }

    inline HDAccountIterator HDAccount_begin() const
    {
        if (m_pFirstHDAccount == NULL)
        {
            return HDAccountIterator(this, m_pEnd);
        }
        return HDAccountIterator(this, m_pFirstHDAccount);
    }

    inline HDAccountIterator HDAccount_end() const
    {
        if (m_pFirstHDAccount == NULL)
        {
            return HDAccountIterator(this, m_pEnd);
        }
        return HDAccountIterator(this, m_pLastHDAccount + 1);
    }

    size_t CountAccounts()   const { return std::count_if(Account_begin(), Account_end(), XLive::IsAccount()); }
    size_t CountMUAccounts() const { return std::count_if(MUAccount_begin(), MUAccount_end(), XLive::IsMUAccount()); }
    size_t CountHDAccounts() const { return std::count_if(HDAccount_begin(), HDAccount_end(), XLive::IsHDAccount()); }

    HRESULT CreateAndSelectGuestAccount(const UserAccountIterator it, int guestNumber);
    HRESULT VerifyLogon(XONLINE_USER const *pLoggedUsers) const;

private:
    XONLINE_USER        m_users[XONLINE_MAX_STORED_ONLINE_USERS];
    XONLINE_USER* const m_pStart;
    XONLINE_USER* const m_pEnd;
    XONLINE_USER        *m_pFirstAccount;
    XONLINE_USER        *m_pLastAccount;
    XONLINE_USER        *m_pFirstMUAccount;
    XONLINE_USER        *m_pLastMUAccount;
    XONLINE_USER        *m_pFirstHDAccount;
    XONLINE_USER        *m_pLastHDAccount;

    XONLINE_USER        m_selectedAccounts[XONLINE_MAX_LOGON_USERS];
    size_t              m_numberOfSelectedAccounts;

    void ComputeAccounts(void);
    void ComputeMUAccounts(void);
    void ComputeHDAccounts(void);



//
/////////////////////// Nested classes
//
public:

    class AccountIterator : public UserAccountIterator
    {
        friend class CXLiveUserManager;

    public:

        inline ~AccountIterator() { ; }

        // Allow conversion to const_AccountIterator.
        inline operator const_AccountIterator() { return const_AccountIterator(m_pUserMgr, m_pUser); }

        // Allow conversion to const *.
        inline operator XONLINE_USER const *() const { return m_pUser; }

        // Allow conversion to *.
        inline operator XONLINE_USER*()         { return m_pUser; }

        // Prefix increment operator
        inline AccountIterator& operator++()
        {
           //Assert(m_pUser != m_pUserMgr->m_pEnd);
           //Assert(m_pUserMgr->IsAccount(m_pUser));
            m_pUser = std::find_if(m_pUser + 1, (m_pUserMgr->m_pLastAccount) + 1, XLive::IsAccount());
            return *this;
        }

        // Prefix decrement operator
        inline AccountIterator& operator--()
        {
           //Assert(m_pUser != m_pUserMgr->m_pStart);
           //Assert(m_pUserMgr->IsAccount(m_pUser));
            do
            {
                --m_pUser;
            } while ((!m_pUserMgr->IsAccount(m_pUser)) && (m_pUser != m_pUserMgr->m_pFirstAccount));
            return *this;
        }

        // Postfix operators
        inline AccountIterator operator--(int)
        {
           //Assert(m_pUser != m_pUserMgr->m_pStart);
            AccountIterator saveIt(*this);
            --(*this);
            return saveIt;
        }

        inline AccountIterator operator++(int)
        {
           //Assert(m_pUser != m_pUserMgr->m_pEnd);
            AccountIterator saveIt(*this);
            ++(*this);
            return saveIt;
        }

    private:
        // Primary constructor
        explicit AccountIterator(CXLiveUserManager const * const pUserMgr, XONLINE_USER *pUser)
            :   UserAccountIterator(pUserMgr, pUser)
        {
           //Assert(NULL != pUserMgr);
           //Assert(NULL != pUser);
        }
    };  // [nested] class AccountIterator


    class const_AccountIterator : public UserAccountIterator
    {
        // The following line allows conversion of AccountIterator to const_AccountIterator.
        friend class AccountIterator;

        friend class CXLiveUserManager;

    public:

        inline const_AccountIterator& operator=(const_AccountIterator const &rhs)
        {
           //Assert(&rhs != this);
            new (this) const_AccountIterator(rhs);
            return *this;
        }

        inline operator XONLINE_USER const *() const    { return m_pUser; }

        inline XONLINE_USER const & operator*() const  { return *m_pUser; }
        inline XONLINE_USER const * operator->() const { return  m_pUser; }

        // Prefix increment operator
        inline const_AccountIterator& operator++()
        {
           //Assert(m_pUser != m_pUserMgr->m_pEnd);
            ++m_pUser;
            return *this;
        }

        // Prefix decrement operator
        inline const_AccountIterator& operator--()
        {
           //Assert(m_pUser != m_pUserMgr->m_pStart);
            --m_pUser;
            return *this;
        }

        // Postfix operators
        inline const_AccountIterator operator--(int)
        {
           //Assert(m_pUser != m_pUserMgr->m_pStart);
            const_AccountIterator saveIt(*this);
            --(*this);
            return saveIt;
        }

        inline const_AccountIterator operator++(int)
        {
           //Assert(m_pUser != m_pUserMgr->m_pEnd);
            const_AccountIterator saveIt(*this);
            ++(*this);
            return saveIt;
        }

    private:
        // Primary constructor
        explicit const_AccountIterator(CXLiveUserManager const * const pUserMgr, XONLINE_USER *pUser)
            :   UserAccountIterator(pUserMgr, pUser)
        {
           //Assert(NULL != pUserMgr);
           //Assert(NULL != pUser);
        }
    };  // [nested] class const_AccountIterator


    class MUAccountIterator : public UserAccountIterator
    {
        friend class CXLiveUserManager;
    public:
        inline ~MUAccountIterator() { ; }

        // Relational operators
        inline bool operator==(MUAccountIterator const &rhs) const  { return rhs.m_pUser == m_pUser; }
        inline bool operator!=(MUAccountIterator const &rhs) const  { return !(rhs == *this); }
        inline bool operator<(MUAccountIterator const &rhs)  const  { return m_pUser < rhs.m_pUser; }   // For STL

        // Prefix increment operator
        inline MUAccountIterator& operator++()
        {
           //Assert(m_pUser != m_pUserMgr->m_pEnd);
           //Assert(m_pUserMgr->IsMUAccount(m_pUser));
            m_pUser = std::find_if(m_pUser + 1, (m_pUserMgr->m_pLastMUAccount) + 1, XLive::IsMUAccount());
            return *this;
        }

        // Prefix decrement operator
        inline MUAccountIterator& operator--()
        {
           //Assert(m_pUser != m_pUserMgr->m_pStart);
           //Assert(m_pUserMgr->IsMUAccount(m_pUser));
            do
            {
                --m_pUser;
            } while ((!m_pUserMgr->IsMUAccount(m_pUser)) && (m_pUser != m_pUserMgr->m_pFirstMUAccount));
            return *this;
        }

        // Postfix operators
        inline MUAccountIterator operator--(int)
        {
           //Assert(m_pUser != m_pUserMgr->m_pStart);
            MUAccountIterator saveIt(*this);
            --(*this);
            return saveIt;
        }

        inline MUAccountIterator operator++(int)
        {
           //Assert(m_pUser != m_pUserMgr->m_pEnd);
            MUAccountIterator saveIt(*this);
            ++(*this);
            return saveIt;
        }

    private:
        // Primary constructor
        inline explicit MUAccountIterator(CXLiveUserManager const * const pUserMgr, XONLINE_USER *pUser)
            :   UserAccountIterator(pUserMgr, pUser)
        {
           //Assert(NULL != pUserMgr);
           //Assert(NULL != pUser);
        }
    };  // [nested] class MUAccountIterator


    class HDAccountIterator : public UserAccountIterator
    {
        friend class CXLiveUserManager;

    public:

        inline ~HDAccountIterator() { ; }

        // Relational operators
        inline bool operator==(HDAccountIterator const &rhs) const  { return rhs.m_pUser == m_pUser; }
        inline bool operator!=(HDAccountIterator const &rhs) const  { return !(rhs == *this); }
        inline bool operator<( HDAccountIterator const &rhs) const  { return m_pUser < rhs.m_pUser; }   // For STL

        // Prefix increment operator
        inline HDAccountIterator& operator++()
        {
           //Assert(m_pUser != m_pUserMgr->m_pEnd);
           //Assert(m_pUserMgr->IsHDAccount(m_pUser));
            m_pUser = std::find_if(m_pUser + 1, (m_pUserMgr->m_pLastHDAccount) + 1, XLive::IsHDAccount());
            return *this;
        }

        // Prefix decrement operator
        inline HDAccountIterator& operator--()
        {
           //Assert(m_pUser != m_pUserMgr->m_pStart);
           //Assert(m_pUserMgr->IsHDAccount(m_pUser));
            do
            {
                --m_pUser;
            } while ((!m_pUserMgr->IsHDAccount(m_pUser)) && (m_pUser != m_pUserMgr->m_pFirstHDAccount));
            return *this;
        }

        // Postfix operators
        inline HDAccountIterator operator--(int)
        {
           //Assert(m_pUser != m_pUserMgr->m_pStart);
            HDAccountIterator saveIt(*this);
            --(*this);
            return saveIt;
        }

        inline HDAccountIterator operator++(int)
        {
           //Assert(m_pUser != m_pUserMgr->m_pEnd);
            HDAccountIterator saveIt(*this);
            ++(*this);
            return saveIt;
        }

    private:
        // Primary constructor
        inline explicit HDAccountIterator(CXLiveUserManager const * const pUserMgr, XONLINE_USER *pUser)
            :   UserAccountIterator(pUserMgr, pUser)
        {
           //Assert(NULL != pUserMgr);
           //Assert(NULL != pUser);
        }
    };  // [nested] class HDAccountIterator

//
///////////////////////
//

};











} // namespace XLive


