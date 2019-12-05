// valac hello.vala --pkg gtk+-3.0
class Application : GLib.Object {
	public static int main(string[] args){
		stdout.printf("hello world\n");

		return 0;
	}
}
