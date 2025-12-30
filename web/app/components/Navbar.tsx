'use client';

import Link from 'next/link';
import { usePathname } from 'next/navigation';

export default function Navbar() {
  const pathname = usePathname();

  const isActive = (path: string) => pathname === path;

  return (
    <nav className="h-[50px] bg-[#252526] border-b border-[#1e1e1e] flex items-center px-4 select-none">
      <div className="flex items-center gap-6 text-[13px]">
        <div className="font-bold text-[#cccccc] flex items-center gap-2 mr-4">
          <span>⛽</span> BENZIN
        </div>
        
        <Link 
          href="/" 
          className={`
            px-3 py-1 rounded-sm transition-colors
            ${isActive('/') ? 'text-white bg-[#37373d]' : 'text-[#969696] hover:text-[#cccccc]'}
          `}
        >
          Edytor
        </Link>
        
        <Link 
          href="/dokumentacja" 
          className={`
            px-3 py-1 rounded-sm transition-colors
            ${isActive('/dokumentacja') ? 'text-white bg-[#37373d]' : 'text-[#969696] hover:text-[#cccccc]'}
          `}
        >
          Dokumentacja
        </Link>
        
        <Link 
          href="/przyklady" 
          className={`
            px-3 py-1 rounded-sm transition-colors
            ${isActive('/przyklady') ? 'text-white bg-[#37373d]' : 'text-[#969696] hover:text-[#cccccc]'}
          `}
        >
          Przykłady
        </Link>
      </div>
    </nav>
  );
}
