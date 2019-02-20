#ifndef _COLOR_
#define _COLOR_

namespace tiara{

	class Color{
		public:
			typedef unsigned char channel;
			channel r;
			channel g;
			channel b;

			Color(channel r_ = 0, channel g_ = 0, channel b_ = 0) : r{r_}, g{g_}, b{b_} {}
			Color(Color & color);
	};

}

#endif