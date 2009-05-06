#ifndef FRAMEDIM_H
#define FRAMEDIM_H

class FrameDim
{
 public:
	FrameDim(int width, int height, Type type);

	int getWidth() const;
	int getHeight() const;
	Type getType() const;
	int getDepth() const;

	static int getTypeBpp(Type type);
	static int getTypeDepth(Type depth);

 private:
	FrameDim();

	int m_width;
	int m_height;
	Type m_type;
	int m_depth;
	
};

inline FrameDim::getWidth() const
{
	return m_width();
}




#endif // FRAMEDIM_H
