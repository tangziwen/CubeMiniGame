#ifndef TZW_APPDELEGATE_H
#define TZW_APPDELEGATE_H


namespace tzw {

class AppEntry
{
public:
    AppEntry();
    virtual void onStart() =0;
    virtual void onExit() =0;
    virtual void onUpdate(float delta) =0;
};

} // namespace tzw

#endif // TZW_APPDELEGATE_H
