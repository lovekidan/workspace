#pragma once

#include <cstdint>
#include <sstream>
#include<Shlwapi.h>
#pragma comment(lib, "shlwapi.lib" )

class GlobalConfig
{
public:
    static GlobalConfig &GetInstance()
    {
        static GlobalConfig config;
        return config;
    }

    bool SetFunctionArgs(const std::string &input, 
        const std::string &output, 
        int outputKind, 
        const int(&ninePatch)[4] = { 0, 0, 0, 0 },
        int quality = 100)
    {
        
        multi_file_ = PathIsDirectoryA(input.c_str()) != 0;
        src_path_ = input;
        dst_path_ = output;
        output_type_ = outputKind;
        output_quality_ = quality;
        nine_path_[0] = ninePatch[0];
        nine_path_[1] = ninePatch[1];
        nine_path_[2] = ninePatch[2];
        nine_path_[3] = ninePatch[3];
        return true;
    }

    const std::string &GetSrcPath()const { return src_path_; }
    const std::string &GetDstPath()const { return dst_path_; }
    bool IsMultiFile()const { return multi_file_; }
    const cv::Vec4i &GetNinePathInfo()const { return nine_path_; }
    uint32_t GetOutputType()const { return output_type_; }
    uint32_t GetOutputQuality()const { return output_quality_; }
    uint32_t GetZoomRatio()const { return zoom_ratio_; }
    inline void setNinePatchInfo(cv::Vec4i &ninePatchInfo){ nine_path_ = ninePatchInfo; }

private:
    GlobalConfig()
        : multi_file_(false)
        , output_type_(1)
        , output_quality_(100)
        , zoom_ratio_(1)
    {
    }


    std::string src_path_;
    std::string dst_path_;
    bool multi_file_;
    cv::Vec4i nine_path_;
    uint32_t output_type_;
    uint32_t output_quality_;
    uint32_t zoom_ratio_;
};
