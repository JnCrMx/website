export module windows:blog;

import std;
import webpp;
import webxx;
import tinyxml2;
import components;

namespace windows {
    using namespace Webxx;

    export Window blog = [](){
        Window w{"blog", "Blog", "images/blog.png", [](){
            return fragment{
                dv{{_id{"blog_container"}}}
            };
        }};

        using namespace webpp::coro;
        w.on_open += []() {
            submit([]()->coroutine<void> {
                std::string xml = co_await fetch("https://wafrn.jcm.re/blog/jcm/rss").then(std::mem_fn(&webpp::response::co_text));
                tinyxml2::XMLDocument doc;
                doc.Parse(xml.data(), xml.size());
                tinyxml2::XMLElement* root = doc.FirstChildElement("rss");
                tinyxml2::XMLElement* channel = root->FirstChildElement("channel");
                for(tinyxml2::XMLElement* item = channel->LastChildElement("item"); item; item = item->PreviousSiblingElement("item")) {
                    std::string title = item->FirstChildElement("title")->GetText();
                    std::string link = item->FirstChildElement("link")->GetText();
                    std::string pubDate = item->FirstChildElement("pubDate")->GetText();
                    std::string description = item->FirstChildElement("description")->GetText();
                    std::string content = item->FirstChildElement("content:encoded")->GetText();
                    using namespace Webxx;
                    auto element = webpp::create_element_from_html(render(dv{
                        h2{{_class{"blog-title"}}, a{{_href{link}, _target{"_blank"}}, title}},
                        dv{i{pubDate}},
                        details{summary{description}, content},
                        hr{},
                    }));
                    webpp::get_element_by_id("blog_container")->append_child(*element);
                }
                co_return;
            }());
        };
        return w;
    }();
}
