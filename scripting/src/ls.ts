import vss from "./vss";

const global = new Function("return this;")();

export interface LocalStorage {
    getItem(key: string, defaultItem?: string): string | null;
    setItem(key: string, value: string): void;
    // same as set but without commit
    putItem(key: string, value: string): void;
    commit(): void;
}

export function ls(): LocalStorage {
    if (global.ls === undefined) {
        global.ls = new (class Impl implements LocalStorage {
            private ls: { [key: string]: string };
            constructor() {
                this.ls = JSON.parse(vss.readLocalStorage());
            }
            getItem(key: string, defaultItem?: string | undefined): string | null {
                return this.ls[key] ?? defaultItem ?? null;
            }
            setItem(key: string, value: string): void {
                this.putItem(key, value);
                this.commit();
            }
            putItem(key: string, value: string): void {
                this.ls[key] = value;
            }
            commit(): void {
                vss.writeLocalStorage(JSON.stringify(this.ls, null, 2));
            }
        });
    }

    return global.ls;
}
