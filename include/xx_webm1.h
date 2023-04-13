///*
//功能：
//	webm 每一帧图展开为 *.png, 或存为 plist 图集, 或直接加载到 cocos 显存 & sprite frame cache
//
//用法流程:
//	1. 准备好 ????? (%d).png 散图, 序号连贯
//	2. ffmpeg 打包成 webm
//	3. 用本类 另存为 xxmv
//
//	4. 实际使用中, 直接加载 xxmv  ( 例如 cocos 下使用 xx_xxmv_cocos.h )
//
//webm 打包命令行:
//	./ffmpeg -f image2 -framerate 60 -i "??????(%d).png" -c:v libvpx-vp9 -pix_fmt yuva420p -b:v 1000K -speed 0 ??????.webm
//
//注意:
//	%d 文件名数字，不可以 中断，从 1 开始
//	60 可以改
//	-b:v 1000K 可以删可以改，从而生成不同体积 webm，可自行斟酌
//	-speed 0 可以删可以改
//
//注意2:
//	图片内容尽量紧凑，边缘不要留太多空格区域，否则会狂吃显存，并降低渲染性能。
//
//
//
//代码示例:
//
//	xx::Webm wm;
//	if (int r = wm.LoadFromWebm("res/a.webm")) return r;		// load a.webm file, parse ebml info, store data
//	if (int r = wm.SaveToXxmv("res/a.xxmv")) return r;			// write xxmv header + data to file
//
//	if (int r = wm.LoadFromXxmv("res/a.xxmv")) return r;		// load a.xxmv file, save info & data
//	if (int r = wm.SaveToPngs("res/", "a")) return r;			// write every frame to a0.png, a1.png, a2.png .... files
//
//	if (int r = wm.SaveToPackedPngs("res/", "a")) return r;		// texture packer likely. write all frame to a?.png + a?.plist
//*/
//
//// todo: 在 webm -> xxmv 这一步中，可以扫描每一帧图片的实际内容包围盒, 找出最大值? 后续生成图集时可优化密度
//// 进一步的, xxmv 每一帧存储 图片尺寸, 实际尺寸, 以便于构造 plist 的时候还原
//
//
//#include "ebml_parser.h"
////#include "xx_data.h"
////#include "xx_file.h"
//#include <filesystem>
//#include <fstream>
//
//namespace xx {
//	// webm 解析后的容易使用的存储形态
//	struct Webm {
//
//        uint8_t codecId = 0;
//
//        // 1: true
//        uint8_t hasAlpha = 0;
//
//        // 像素宽
//        uint16_t width = 0;
//
//        // 像素高
//        uint16_t height = 0;
//
//        // 总播放时长( 秒 )
//        float duration = 0;
//
//        // 所有帧数据长度集合( hasAlpha 则为 rgb / a 交替长度集合 )
//        std::vector<uint32_t> lens;
//
//        // 所有帧数据依次排列
////        xx::Data data;
//
//        // 所有帧 rgb / a 数据块指针( 后期填充 )
//        std::vector<uint8_t*> bufs;
//
//        // 总帧数( 后期填充 )
//        uint32_t count = 0;
//
//        inline int Init() {
//            count = (uint32_t)(hasAlpha ? lens.size() / 2 : lens.size());
//
//            bufs.resize(lens.size());
////            auto baseBuf = data.buf;
////            for (int i = 0; i < lens.size(); ++i) {
////                bufs[i] = (uint8_t*)baseBuf;
////                baseBuf += lens[i];
////            }
////            if (baseBuf != data.buf + data.len) return __LINE__;
//            return 0;
//        }
//
//        inline void Clear() {
//            codecId = 0;
//            hasAlpha = 0;
//            width = 0;
//            height = 0;
//            duration = 0;
//            lens.clear();
////            data.Clear();
//            bufs.clear();
//            count = 0;
//        }
//
//        std::vector<char> LoadFile(const std::string& path) {
//            std::vector<char> bytes;
//            std::ifstream ifs(path, std::ios::in|std::ios::binary|std::ios::ate);
//            auto pos = ifs.tellg();
//            int length = pos;
////            char *pChars = new char[length];
//            ifs.seekg(0, std::ios::beg);
//            bytes.resize(length);
//            ifs.read(bytes.data(), length);
//            ifs.close();
//            return bytes;
//        }
//
//		// 从 .webm 读出数据并填充到 wm. 成功返回 0
//		inline int LoadFromWebm(const std::string& path) {
//			this->Clear();
//            auto dataVec = LoadFile(path);
//			// 读文件
////			std::unique_ptr<uint8_t[]> data;
////			size_t dataLen;
////			if (int r = xx::ReadAllBytes(path, data, dataLen)) return __LINE__;
////        inline int LoadFromWebm(std::unique_ptr<uint8_t[]> data) {
////			// 开始解析 ebml 头
//            size_t dataLen = dataVec.size();
//			auto&& ebml = parse_ebml_file((uint8_t*)dataVec.data(), dataLen, 1);
//			auto&& segment = ebml.FindChildById(EbmlElementId::Segment);
//
//			// 提取 播放总时长
//			auto&& info = segment->FindChildById(EbmlElementId::Info);
//			auto&& duration = info->FindChildById(EbmlElementId::Duration);
//			this->duration = (float)std::stod(duration->value());
//
//			// 提取 编码方式
//			auto&& tracks = segment->FindChildById(EbmlElementId::Tracks);
//			auto&& trackEntry = tracks->FindChildById(EbmlElementId::TrackEntry);
//			auto&& codecId = trackEntry->FindChildById(EbmlElementId::CodecID);
//			this->codecId = codecId->value() == "V_VP8" ? 0 : 1;
//
//			// 提取 宽高
//			auto&& video = trackEntry->FindChildById(EbmlElementId::Video);
//			auto&& pixelWidth = video->FindChildById(EbmlElementId::PixelWidth);
//			this->width = std::stoi(pixelWidth->value());
//			auto&& pixelHeight = video->FindChildById(EbmlElementId::PixelHeight);
//			this->height = std::stoi(pixelHeight->value());
//
//			// 判断 是否带 alpha 通道
//			auto&& _alphaMode = video->FindChildById(EbmlElementId::AlphaMode);
//			this->hasAlpha = _alphaMode->value() == "1" ? 1 : 0;
//
//			std::vector<int> frames;
//			uint32_t frameNumber = 0;
//
//			std::list<EbmlElement>::const_iterator clusterOwner;
//			if (this->codecId == 0) {
//				clusterOwner = segment;
//			}
//			else {
//				auto&& tags = segment->FindChildById(EbmlElementId::Tags);
//				auto&& tag = tags->FindChildById(EbmlElementId::Tag);
//				clusterOwner = tag->FindChildById(EbmlElementId::Targets);
//			}
//
//			auto&& cluster = clusterOwner->FindChildById(EbmlElementId::Cluster);
//			while (cluster != clusterOwner->children().cend()) {
//				auto timecode = cluster->FindChildById(EbmlElementId::Timecode);
//				auto clusterPts = std::stoi(timecode->value());
//
//				if (this->hasAlpha) {
//					auto&& blockGroup = cluster->FindChildById(EbmlElementId::BlockGroup);
//					while (blockGroup != cluster->children().cend()) {
//						{
//							// get yuv data + size
//							auto&& block = blockGroup->FindChildById(EbmlElementId::Block);
//							auto&& data = block->data();
//							auto&& size = block->size();
//
//							// fix yuv data + size
//							size_t track_number_size_length;
//							(void)get_ebml_element_size(data, size, track_number_size_length);
//							data = data + track_number_size_length + 3;
//							size = size - track_number_size_length - 3;
//
//							this->lens.push_back((uint32_t)size);
////							this->data.WriteBuf(data, size);
//						}
//						{
//							// get a data + size
//							auto&& blockAdditions = blockGroup->FindChildById(EbmlElementId::BlockAdditions);
//							auto&& blockMore = blockAdditions->FindChildById(EbmlElementId::BlockMore);
//							auto&& blockAdditional = blockMore->FindChildById(EbmlElementId::BlockAdditional);
//							auto&& data_alpha = blockAdditional->data();
//							auto&& size_alpha = (uint32_t)blockAdditional->size();
//
//							this->lens.push_back((uint32_t)size_alpha);
////							this->data.WriteBuf(data_alpha, size_alpha);
//						}
//
//						// next
//						blockGroup = cluster->FindNextChildById(++blockGroup, EbmlElementId::BlockGroup);
//						++frameNumber;
//					}
//				}
//				else {
//					auto&& simpleBlock = cluster->FindChildById(EbmlElementId::SimpleBlock);
//					while (simpleBlock != cluster->children().cend()) {
//						auto&& data = simpleBlock->data();
//						auto&& size = simpleBlock->size();
//
//						// fix yuv data + size
//						size_t track_number_size_length;
//						(void)get_ebml_element_size(data, size, track_number_size_length);
//						data = data + track_number_size_length + 3;
//						size = size - track_number_size_length - 3;
//
//						this->lens.push_back((uint32_t)size);
////						this->data.WriteBuf(data, size);
//
//						// next
//						simpleBlock = cluster->FindNextChildById(++simpleBlock, EbmlElementId::BlockGroup);
//						++frameNumber;
//					}
//				}
//
//				cluster = clusterOwner->FindNextChildById(++cluster, EbmlElementId::Cluster);
//			}
//
//            return Init();
//		}
//
//
//
//	};
//}
//
//
////
////std::string FloatToString(float const& v) {
////	std::string s(15, '\0');
////	s.resize(sprintf(s.data(), "%g", v));
////	return s;
////}
////PListMaker& Append(std::string_view const& name
////	, float const& soX, float const& soY	/* spriteOffset */
////	, int const& ssW, int const& ssH		/* spriteSize */
////	, int const& sssW, int const& sssH		/* spriteSourceSize */
////	, int const& trX, int const& trY, int const& trW, int const& trH		/* textureRect */
////	/*, bool const& textureRotated*/
////) {
////	data += std::string(R"(
////            <key>)") + std::string(name) + R"(</key>
////            <dict>
////                <key>aliases</key>
////                <array/>
////                <key>spriteOffset</key>
////                <string>{)" + FloatToString(soX) + "," + FloatToString(soY) + R"(}</string>
////                <key>spriteSize</key>
////                <string>{)" + std::to_string(ssW) + "," + std::to_string(ssH) + R"(}</string>
////                <key>spriteSourceSize</key>
////                <string>{)" + std::to_string(sssW) + "," + std::to_string(sssH) + R"(}</string>
////                <key>textureRect</key>
////				<string>{{)" + std::to_string(trX) + "," + std::to_string(trY) + "},{" + std::to_string(trW) + "," + std::to_string(trH) + R"(}}</string>
////                <key>textureRotated</key>
////                <false/>
////            </dict>)";
////	return *this;
////}
