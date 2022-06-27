#include <iostream>
#include <vector>
#include <cmath>
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

#define IMAGE_PATH "/works/pig_weight_cpp/data/line_img.jpg"

using namespace std;

/**
 * @brief
 *
 * @param input
 * @param pts1
 * @param pts2
 * @return int
 */
int calc_pt_matrix(cv::Point2f input[4], cv::Point2f pts1[4], cv::Point2f pts2[4])
{

    vector<float> xpy;
    vector<float> xmy;
    // cv::Point2f pts1[4];
    // cv::Point2f pts2[4];

    xpy.push_back(input[0].x + input[0].y);
    xpy.push_back(input[1].x + input[1].y);
    xpy.push_back(input[2].x + input[2].y);
    xpy.push_back(input[3].x + input[3].y);

    xmy.push_back(input[0].y - input[0].x);
    xmy.push_back(input[1].y - input[1].x);
    xmy.push_back(input[2].y - input[2].x);
    xmy.push_back(input[3].y - input[3].x);

    int tl_idx = min_element(xpy.begin(), xpy.end()) - xpy.begin();
    int br_idx = max_element(xpy.begin(), xpy.end()) - xpy.begin();
    int tr_idx = min_element(xmy.begin(), xmy.end()) - xmy.begin();
    int bl_idx = max_element(xmy.begin(), xmy.end()) - xmy.begin();

    pts1[0] = input[tl_idx];
    pts1[1] = input[tr_idx];
    pts1[2] = input[br_idx];
    pts1[3] = input[bl_idx];

    float w1 = abs(input[br_idx].x - input[bl_idx].x);
    float w2 = abs(input[tr_idx].x - input[tl_idx].x);
    float h1 = abs(input[tr_idx].y - input[br_idx].y);
    float h2 = abs(input[tl_idx].y - input[bl_idx].y);
    float width = max(w1, w2);
    float height = max(h1, h2);
    float pl = min(input[tl_idx].x, input[bl_idx].x);
    float pt = min(input[tl_idx].y, input[tr_idx].y);

    pts2[0] = cv::Point2f(pl, pt);
    pts2[1] = cv::Point2f(pl + width - 1, pt);
    pts2[2] = cv::Point2f(pl + width - 1, pt + height - 1);
    pts2[3] = cv::Point2f(pl, pt + height - 1);

    return 1;
}

bool compare_func(cv::Point2f a, cv::Point2f b)
{
    if (a.x < b.x)
    {
        return true;
    }
    else if (a.x == b.x)
    {
        if (a.y < b.y)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

int main()
{

    //* INIT
    cv::Point2f inputQuad[4];
    cv::Point2f pts1[4];
    cv::Point2f pts2[4];
    cv::Mat mtrx(2, 4, CV_64FC1);

    cv::Mat img, out;

    img = cv::imread(IMAGE_PATH, cv::IMREAD_COLOR);

    //이미지를 정상적으로 읽어왔는지 확인
    if (img.empty())
    {
        cout << IMAGE_PATH << " ERROR" << endl;
        return -1;
    }

    //* Process
    //* line dot search ---------------------------------------------
    vector<cv::Point2f> line_point;
    vector<int> y_line_point;
    for (int row = 0; row < img.rows; row++)
    {
        uchar *pointer_row = img.ptr<uchar>(row);
        for (int col = 0; col < img.cols; col++)
        {
            int b = static_cast<int>(pointer_row[col * 3 + 0]);
            int g = static_cast<int>(pointer_row[col * 3 + 1]);
            int r = static_cast<int>(pointer_row[col * 3 + 2]);

            if (r > 245 && g < 10 && b < 10)
            {
                // printf("\t (%d, %d, %d)", r, g, b);
                // cout << r << " " << g << " " << b << endl;
                // cout << row << "  " << col << endl;
                line_point.push_back(cv::Point2f(col, row));
                y_line_point.push_back(row);
            }
        }
    }
    sort(line_point.begin(), line_point.end(), compare_func);
    int yline_max = *max_element(y_line_point.begin(), y_line_point.end());
    int yline_min = *min_element(y_line_point.begin(), y_line_point.end());

    cout << yline_max << " " << yline_min << endl;
    float center_line = (yline_max + yline_min) / 2;

    vector<cv::Point2f> xy_point_top;
    vector<cv::Point2f> xy_point_bot;

    for (int i = 0; i < line_point.size(); i++)
    {
        if (i == 0)
        {
            if (line_point[i].y < center_line)
            {
                xy_point_top.push_back(line_point[i]);
            }
            else
            {
                xy_point_bot.push_back(line_point[i]);
            }
        }
        else if (i == line_point.size() - 1)
        {
            if (line_point[i].y < center_line)
            {
                xy_point_top.push_back(line_point[i]);
            }
            else
            {
                xy_point_bot.push_back(line_point[i]);
            }
        }
        else
        {
            if (line_point[i].x - line_point[i - 1].x > 100)
            {
                if (line_point[i - 1].y < center_line)
                {
                    xy_point_top.push_back(line_point[i - 1]);
                }
                else
                {
                    xy_point_bot.push_back(line_point[i - 1]);
                }
                if (line_point[i].y < center_line)
                {
                    xy_point_top.push_back(line_point[i]);
                }
                else
                {
                    xy_point_bot.push_back(line_point[i]);
                }
            }
        }
        // cout << line_point[i].x << " " << line_point[i].y << endl;
    }

    for (int i = 0; i < static_cast<int>(xy_point_top.size()); i++)
    {
        cout << "[TOP] point : " << xy_point_top[i].x << " " << xy_point_top[i].y << endl;
    }

    for (int i = 0; i < static_cast<int>(xy_point_bot.size()); i++)
    {
        cout << "[BOT] point : " << xy_point_bot[i].x << " " << xy_point_bot[i].y << endl;
    }
    //* ---------------------------------------------------------------------------------

    //* Perspective transform -----------------------------------------------------------
    inputQuad[0] = xy_point_top[0]; // top-left
    inputQuad[1] = xy_point_top[4]; // top-right
    inputQuad[2] = xy_point_bot[4]; // bottom-right
    inputQuad[3] = xy_point_bot[0]; // bottom-left

    calc_pt_matrix(inputQuad, pts1, pts2);

    mtrx = cv::getPerspectiveTransform(pts1, pts2);
    cv::warpPerspective(img, out, mtrx, img.size());

    cv::rectangle(out, cv::Rect(cv::Point(pts2[0].x, pts2[0].y), cv::Point(pts2[2].x, pts2[2].y)), cv::Scalar(0, 0, 255), 3, 8, 0);

    cv::imwrite("out_sample.jpg", out);

    return 0;
}