
struct MyCatId : public CATID
{
	inline MyCatId(const CATID &catid) {memcpy((CATID *) this, &catid, sizeof(CATID));}
};