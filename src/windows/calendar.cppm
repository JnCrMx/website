export module windows:calendar;

import std;
import webpp;
import webxx;
import components;

namespace windows {
    using namespace Webxx;

    export Window calendar = [](){
        Window w{"calendar", "Events", "images/calendar.png", [](){
            return fragment{
                p{"A list of events I am planning to attend and places I will go to:"},
                ul{{_id{"calendar_container"}}}
            };
        }};

        using namespace webpp::coro;
        w.on_open += []() {
            submit([]()->coroutine<void> {
                std::string cal = co_await fetch("https://cal.jcm.re/jcm/public").then(std::mem_fn(&webpp::response::co_text));

                struct simple_event {
                    std::chrono::year_month_day begin{}, end{};
                    std::string summary, location, url, description;
                    bool tentative{false};

                    bool operator<(const simple_event& o) const {
                        return begin < o.begin;
                    }
                    bool valid() const {
                        return !summary.empty() && begin.ok() && end.ok();
                    }
                };
                std::vector<simple_event> events;

                for(auto pevent : std::views::split(cal, std::string_view{"BEGIN:VEVENT"})) {
                    std::string_view event{pevent};
                    if(event.starts_with("BEGIN:")) {
                        continue;
                    }

                    simple_event e;
                    for(auto pline : std::views::split(event, std::string_view{"\r\n"})) {
                        std::string_view line{pline};
                        if(line.starts_with("SUMMARY:")) {
                            e.summary = line.substr(std::char_traits<char>::length("SUMMARY:"));
                        }
                        else if(line.starts_with("LOCATION:")) {
                            e.location = line.substr(std::char_traits<char>::length("LOCATION:"));
                        }
                        else if(line.starts_with("DTSTART;VALUE=DATE:")) {
                            auto date = line.substr(std::char_traits<char>::length("DTSTART;VALUE=DATE:"));
                            int year = std::stoi(std::string{date.substr(0, 4)});
                            int month = std::stoi(std::string{date.substr(4, 2)});
                            int day = std::stoi(std::string{date.substr(6, 2)});
                            e.begin = std::chrono::year_month_day{std::chrono::year(year), std::chrono::month(month), std::chrono::day(day)};
                        }
                        else if(line.starts_with("DTEND;VALUE=DATE:")) {
                            auto date = line.substr(std::char_traits<char>::length("DTEND;VALUE=DATE:"));
                            int year = std::stoi(std::string{date.substr(0, 4)});
                            int month = std::stoi(std::string{date.substr(4, 2)});
                            int day = std::stoi(std::string{date.substr(6, 2)});
                            auto ymd = std::chrono::year_month_day{std::chrono::year(year), std::chrono::month(month), std::chrono::day(day)};
                            e.end = std::chrono::year_month_day{std::chrono::sys_days{ymd} - std::chrono::days(1)};
                        }
                        else if(line.starts_with("URL:")) {
                            e.url = line.substr(std::char_traits<char>::length("URL:"));
                        }
                        else if(line.starts_with("DESCRIPTION:")) {
                            e.description = line.substr(std::char_traits<char>::length("DESCRIPTION:"));
                        }
                        else if(line == "STATUS:TENTATIVE") {
                            e.tentative = true;
                        }
                    }

                    if(!e.valid()) {
                        continue;
                    }

                    events.push_back(std::move(e));
                }

                std::ranges::sort(events, std::less<>{});

                for(auto e : events) {
                    using namespace Webxx;
                    auto element = webpp::create_element_from_html(render(li{
                        (e.begin == e.end ?
                            fragment{
                                std::format("{}", e.begin)
                            } :
                            fragment{
                                std::format("{}", e.begin),
                                " to ",
                                std::format("{}", e.end)
                            }
                        ),
                        ": ",
                        !e.url.empty() ? fragment{a{{_href{e.url}, _target{"_blank"}}, b{e.summary}}} : fragment{b{e.summary}},
                        !e.description.empty() ? fragment{" ", small{e.description}} : fragment{},
                        !e.location.empty() ? fragment{" in ", b{e.location}} : fragment{},
                        e.tentative ? fragment{" ", i{"(hopefully)"}} : fragment{},
                    }));
                    webpp::get_element_by_id("calendar_container")->append_child(*element);
                }
            }());
        };
        return w;
    }();
}
